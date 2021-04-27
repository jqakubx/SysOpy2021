#define _POSIX_C_SOURCE 200809L
#include<stdio.h>
#include<stdlib.h>

#include<string.h>
#include<signal.h>
#include"chatconfig.h"
#include<unistd.h>
#include<time.h>
#include<signal.h>
#include<sys/time.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<mqueue.h>

int id;
mqd_t my_id = -1;
mqd_t server_id = -1;
int connected_id = -1;
char name_of_file[FILE_L];
void quit_f(int singal) {
    printf("Stop this client\n");
    char* quit_tx = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    sprintf(quit_tx, "%d", id);
    mq_send(server_id, quit_tx, BIGGEST_MESSAGE, STOP);
    mq_close(my_id);
    mq_close(server_id);
    mq_unlink(name_of_file);
    exit(0);
}
void quit_f2(){
    printf("Stop this client\n");
    char* quit_tx = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    sprintf(quit_tx, "%d", id);
    mq_send(server_id, quit_tx, BIGGEST_MESSAGE, STOP);
    mq_close(my_id);
    mq_close(server_id);
    mq_unlink(name_of_file);
    exit(0);
}


void chat(mqd_t client_connected) {
    
  
    char* message_line = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    ssize_t size = 0;
    size_t len = 0;
    char* to_send = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    char* server_msg = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    printf("Chat with %d\n", connected_id);
    printf("Write DISCONNECT to disconnect from this chat\n");
    printf("If you want to read messages press enter\n");
    int end = 1;
    while(1 && end) {
        unsigned int frm;
        printf("[YOU]: ");
        size = getline(&message_line, &len, stdin);
        message_line[size-1] = '\0';
        struct timespec* timespec = (struct timespec*)malloc(sizeof(struct timespec));
        while(mq_timedreceive(my_id, server_msg, BIGGEST_MESSAGE, &frm, timespec) >= 0) {
            if (frm == STOP) {
                printf("End of program\n");
                quit_f(0);
            }
            else if (frm == DISCONNECT) {
                mq_close(client_connected);
                printf("End  of connection\n");
                connected_id = -1;
                end = 0;
                break;
            }
            else if (frm == 6) {
                printf("[OTHER]: %s\n", server_msg);
            }
        }

        if(strcmp(message_line, "DISCONNECT") == 0) {
            mq_close(client_connected);
            printf("End of this chat\n");
            sprintf(to_send, "%d", id);
            mq_send(server_id, to_send, BIGGEST_MESSAGE, DISCONNECT);
            break;
        } else {
            
            sprintf(to_send, "%s", message_line);
            if(strcmp(message_line, "") != 0)
                mq_send(client_connected, to_send, BIGGEST_MESSAGE, 6);
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
    char* from_server = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    struct timespec* timespec = (struct timespec*)malloc(sizeof(struct timespec));
    unsigned int type_msg;
    if (mq_timedreceive(my_id, from_server, BIGGEST_MESSAGE, &type_msg, timespec) < 0) return;
    else if (type_msg == STOP) {
        printf("Server is stoped. We have to close client %d\n", id);
        quit_f(0);
    } else if (type_msg == CONNECT) {
        
        char *part_msg = strtok(from_server, " ");
        connected_id = atoi(part_msg);
        char* queue_other = strtok(NULL," ");
        
        mqd_t queue_other_cnt = mq_open(queue_other, O_WRONLY, 0666);
        chat(queue_other_cnt);
    }
}


int main() {

    signal(SIGINT, quit_f);
    
    sprintf(name_of_file, "/%s%d", "file", getpid());

    struct mq_attr attr_client;
    attr_client.mq_maxmsg = MAX_COM;
    attr_client.mq_msgsize = BIGGEST_MESSAGE;
    
    if((my_id = mq_open(name_of_file, O_RDONLY | O_CREAT , 0666, &attr_client)) == -1) {
        printf("ERROR - %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    if((server_id = mq_open(SERVER_NAME, O_WRONLY)) == -1) {
        printf("ERROR - %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    
    char* initialize = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    strcpy(initialize, name_of_file);
    
    mq_send(server_id, initialize, BIGGEST_MESSAGE, INIT);
    char* back = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    unsigned int back_r;
    mq_receive(my_id, back, BIGGEST_MESSAGE, &back_r);
    id = atoi(back);
    printf("Initialized client with id: %d\n", id);
    char* message_line = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    ssize_t size = 0;
    size_t len = 0;
    //atexit(quit_f2);
    while(1) {
        printf("Choose command: \n");
        print_commands();

        size = getline(&message_line, &len, stdin);
        if(size > 1)
            message_line[size - 1] = '\0';
        printf("%s", message_line);
        
        server_comunicate();
        

        char *part_msg = strtok(message_line, " ");
        if (strcmp(part_msg, "LIST") == 0) {
            unsigned int wait_rcv;
            char* list_msg = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
            sprintf(list_msg, "%d", id);
            mq_send(server_id, list_msg, BIGGEST_MESSAGE, LIST);

            char* list_msg_back = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
            mq_receive(my_id, list_msg_back, BIGGEST_MESSAGE, &wait_rcv);
            printf("Clients to connected:\n");
            printf("%s\n", list_msg_back);
        }
        else if (strcmp(part_msg, "CONNECT") == 0) {
            part_msg = strtok(NULL, " ");
            int connect_id = atoi(part_msg);
            char* m_connection = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
        
            
            sprintf(m_connection, "%d %d", id, connect_id);
            
            mq_send(server_id, m_connection, BIGGEST_MESSAGE, CONNECT);

            unsigned int back_cc;
            char* m_connection_back = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
            mq_receive(my_id, m_connection_back, BIGGEST_MESSAGE, &back_cc);
            connected_id = connect_id;
            mqd_t connected_queue = mq_open(m_connection_back, O_WRONLY, 0666);
            
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


