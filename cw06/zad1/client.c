#define _XOPEN_SOURCE 700
#include<stdio.h>
#include<stdlib.h>
#include<sys/msg.h>
#include<sys/types.h>
#include<string.h>
#include<signal.h>
#include"chatconfig.h"
#include<unistd.h>
#include<time.h>
#include<signal.h>
#include<sys/time.h>
#include<unistd.h>

int id;
int my_id;
int server_id;
int connected_id = -1;

void quit_f(int singal) {
    printf("Stop this client\n");
    message quit;
    quit.m_type = STOP;
    sprintf(quit.m_message, "%d", id);
    msgsnd(server_id, &quit, BIGGEST_MESSAGE, 0);
    msgctl(my_id, IPC_RMID, NULL);
    exit(0);
}
void quit_f2(){
    printf("Stop this client\n");
    message quit;
    quit.m_type = STOP;
    sprintf(quit.m_message, "%d", id);
    msgsnd(server_id, &quit, BIGGEST_MESSAGE, 0);
    msgctl(my_id, IPC_RMID, NULL);
    exit(0);
}


void chat(int connected_queue) {
    
  
    char* message_line;
    ssize_t size = 0;
    size_t len = 0;
    message to_connected;
    message server_msg;
    printf("Chat with %d\n", connected_id);
    printf("Write DISCONNECT to disconnect from this chat\n");
    printf("If you want to read messages press enter\n");
    while(1) {
        printf("[YOU]: ");
        size = getline(&message_line, &len, stdin);
        message_line[size-1] = '\0';
        if(msgrcv(my_id, &server_msg, BIGGEST_MESSAGE, STOP, IPC_NOWAIT) >= 0) {
            printf("End of program\n");
            quit_f(0);
        }
        if(msgrcv(my_id, &server_msg, BIGGEST_MESSAGE, DISCONNECT, IPC_NOWAIT) >= 0) {
            printf("End  of connection\n");
            connected_id = -1;
            break;
        }
        while(msgrcv(my_id, &to_connected, BIGGEST_MESSAGE, 6, IPC_NOWAIT) >= 0) {
            printf("[OTHER]: %s\n", to_connected.m_message);
        }
        if(strcmp(message_line, "DISCONNECT") == 0) {
            printf("End of this chat\n");
            server_msg.m_type = DISCONNECT;
            sprintf(server_msg.m_message, "%d", id);
            msgsnd(server_id, &server_msg, BIGGEST_MESSAGE, 0);
            break;
        } else {
            to_connected.m_type = 6;
            sprintf(to_connected.m_message, "%s", message_line);
            if(strcmp(message_line, "") != 0)
                msgsnd(connected_queue, &to_connected, BIGGEST_MESSAGE, 0);
        }
        
    }
}

void print_commands() {
    printf("LIST - show list of available clients to connect\n");
    printf("CONNECT id - connect to client with given id\n");
    printf("STOP -stop this client program\n");
    printf("Press Enter to wait for STOP or CONNECT\n");
}

void server_comunicate() {
    message from_server;
    if (msgrcv(my_id, &from_server, BIGGEST_MESSAGE, 0, IPC_NOWAIT) < 0) return;
    else if (from_server.m_type == STOP) {
        printf("Server is stoped. We have to close client %d", id);
        quit_f(0);
    } else if (from_server.m_type == CONNECT) {
        char *part_msg = strtok(from_server.m_message, " ");
        connected_id = atoi(part_msg);
        int queue_other = atoi(strtok(NULL," "));
        chat(queue_other);
    }
}


int main() {
    key_t queue_server_key = ftok(getenv("HOME"), ID_OF_SERVER);

    server_id = msgget(queue_server_key, 0666);

    key_t my_queue_key = ftok(getenv("HOME"), getpid());

    my_id = msgget(my_queue_key, IPC_CREAT | 0666);

    signal(SIGINT, quit_f);

    message initialize;
    initialize.m_type = INIT;
    sprintf(initialize.m_message, "%d", my_id);
    msgsnd(server_id, &initialize, BIGGEST_MESSAGE, 0);
    message back;
    msgrcv(my_id, &back, BIGGEST_MESSAGE, 0, 0);
    id = atoi(back.m_message);
    printf("Initialized client with id: %d\n", id);
    char* message_line;
    ssize_t size = 0;
    size_t len = 0;
    atexit(quit_f2);
    while(1) {
        printf("Choose command: \n");
        print_commands();
        size = getline(&message_line, &len, stdin);
        if(size > 1)
            message_line[size - 1] = '\0';

        server_comunicate();
        

        char *part_msg = strtok(message_line, " ");
        if (strcmp(part_msg, "LIST") == 0) {
            message list_msg;
            list_msg.m_type = LIST;
            sprintf(list_msg.m_message, "%d", id);
            msgsnd(server_id, &list_msg, BIGGEST_MESSAGE, 0);
            message list_msg_back;
            msgrcv(my_id, &list_msg_back, BIGGEST_MESSAGE, LIST, 0);
            printf("Clients to connected:\n");
            printf("%s\n", list_msg_back.m_message);
        }
        else if (strcmp(part_msg, "CONNECT") == 0) {
            part_msg = strtok(NULL, " ");
            int connect_id = atoi(part_msg);
            message m_connection;
            m_connection.m_type = CONNECT;
            sprintf(m_connection.m_message, "%d %d", id, connect_id);
            
            msgsnd(server_id, &m_connection, BIGGEST_MESSAGE, 0);
            message m_connection_back;
            msgrcv(my_id, &m_connection_back, BIGGEST_MESSAGE, CONNECT, 0);
            connected_id = connect_id;
            int connected_queue = atoi(m_connection_back.m_message);
            
            printf("Connect with client %d\n", connected_id);
            chat(connected_queue);
        }
        else if (strcmp(part_msg, "STOP") == 0) {
            quit_f(0);
        }
        else {
            printf("Nothing\n");
        }
    }
    return 0;
}


