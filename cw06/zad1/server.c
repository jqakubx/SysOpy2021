#include<stdio.h>
#include<stdlib.h>
#include<sys/msg.h>
#include<sys/types.h>
#include<string.h>
#include"chatconfig.h"
#include<signal.h>
#include<unistd.h>

client* all_clients[MAX_CLIENTS_NR];
int queue_server;
int nr_of_clients = 0;
int current_client_add = 0;
int current_client = 0;


client* get_sender_client(int id) {
    for(int i = 0; i < current_client; i++) {
        if (all_clients[i]->client_id == id)
            return all_clients[i];
    }
    return NULL;
}

void stop_f(message *m_mgs) {
    printf("End of client %s\n", m_mgs->m_message);
    int id = atoi(m_mgs->m_message);
    client *sender = get_sender_client(id);
    int id_table;
    for(int i = 0; i < current_client; i++){
        if (all_clients[i]->client_id == id)
            id_table = i;
    }
    free(sender);
    for(int i = id_table; i < current_client; i++)
        all_clients[i] = all_clients[i+1];
    all_clients[current_client-1] = NULL;
    current_client--;
}

void quit_f2(){
    printf("End of server\n");
    message server_end;
    message back;
    server_end.m_type = STOP;
    for(int i = 0; i < current_client; i++) {
        msgsnd(all_clients[i]->client_queue, &server_end, BIGGEST_MESSAGE, 0);
    }
    while(current_client > 0) {
        msgrcv(queue_server, &back, BIGGEST_MESSAGE, STOP, 0);
        
        stop_f(&back);
    }
    msgctl(queue_server, IPC_RMID, NULL);
    exit(0);
}

void quit_f(int signal) {
    printf("End of server\n");
    message server_end;
    message back;
    server_end.m_type = STOP;
    for(int i = 0; i < current_client; i++) {
        msgsnd(all_clients[i]->client_queue, &server_end, BIGGEST_MESSAGE, 0);
    }
    while(current_client > 0) {
        msgrcv(queue_server, &back, BIGGEST_MESSAGE, STOP, 0);
        
        stop_f(&back);
    }
    msgctl(queue_server, IPC_RMID, NULL);
    exit(0);
}

void init_f(message *m_msg) {
    
    int new_queue = atoi(m_msg -> m_message);

    client* new_client = (client*)malloc(sizeof(client));
    new_client->client_id=current_client_add++;
    new_client->client_queue = new_queue;
    new_client->connection = -1;
    

    all_clients[current_client] = new_client;
    current_client++;
    printf("Initialize new client: %d\n", new_client->client_id);

    message to_client;
    to_client.m_type = INIT;
    sprintf(to_client.m_message, "%d", new_client->client_id);
    msgsnd(new_queue, &to_client, BIGGEST_MESSAGE, 0);
}

void list_f(message *m_msg) {
    
    int id_sender = atoi(m_msg->m_message);
    client* client_sender = get_sender_client(id_sender);

    message to_client;
    strcpy(to_client.m_message, "");
    to_client.m_type = LIST;
    int counter = 0;
    for(int i = 0; i < current_client; i++) {
        if(all_clients[i]->client_id != id_sender) {
            sprintf(to_client.m_message + strlen(to_client.m_message), "%d - status %d\n", all_clients[i]->client_id,
            all_clients[i]->connection == -1);
            counter++;
        }
        
    }
    if (counter == 0)
        sprintf(to_client.m_message, "Lack of available clients\n");
    printf("Send list of clients to client %d\n", atoi(m_msg->m_message));
    msgsnd(client_sender->client_queue, &to_client, BIGGEST_MESSAGE, 0);
}

void connect_f(message *m_msg) {
    
    int id_client = atoi(strtok(m_msg->m_message, " "));
    client* from = get_sender_client(id_client);
    int id_to_connected = atoi(strtok(NULL, " " ));
    
    client* to = get_sender_client(id_to_connected);
    from->connection = to->client_id;
    to->connection = from->client_id;
    printf("Connect client %d with %d\n", from->client_id, to->client_id);

    message to_clients;
    message to_clients2;
    to_clients.m_type = CONNECT;
    to_clients2.m_type = CONNECT;
    sprintf(to_clients.m_message, "%d", to->client_queue);
    msgsnd(from->client_queue, &to_clients, BIGGEST_MESSAGE, 0);
    
    sprintf(to_clients2.m_message, "%d %d", from->client_id, from->client_queue);
    msgsnd(to->client_queue, &to_clients2, BIGGEST_MESSAGE, 0);
}

void disconnect_f(message *m_msg) {
    
    int sender = atoi(m_msg->m_message);

    client *from = get_sender_client(sender);
    client *to = get_sender_client(from->connection);
    printf("Disconnecting %d and %d.\n", from->client_id, to->client_id);
    from->connection = -1;
    to->connection = -1;

    message to_client;
    to_client.m_type = DISCONNECT;
    msgsnd(to->client_queue, &to_client, BIGGEST_MESSAGE, 0);
}


int main(){
    printf("Start of server\n");
    key_t queue_server_key = ftok(getenv("HOME"), ID_OF_SERVER);

    queue_server = msgget(queue_server_key, IPC_CREAT | 0666);

    signal(SIGINT, quit_f);
    atexit(quit_f2);
    while(1) {
        message m_msg;
        msgrcv(queue_server, &m_msg, BIGGEST_MESSAGE, -5, 0);
        switch(m_msg.m_type) {
            case INIT:
                init_f(&m_msg);
                break;
            case LIST:
                list_f(&m_msg);
                break;
            case CONNECT:
                connect_f(&m_msg);
                break;
            case DISCONNECT:
                disconnect_f(&m_msg);
                break;
            case STOP:
                stop_f(&m_msg);
                break;  
            default:
                printf("BAD TYPE OF MESSAGE\n");
        }
    }
}
