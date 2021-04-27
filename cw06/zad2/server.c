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

int all_clients_id[MAX_CLIENTS_NR];
mqd_t all_clients_mqd[MAX_CLIENTS_NR];
char* all_clients_path[MAX_CLIENTS_NR];
int connection_clients[MAX_CLIENTS_NR];
mqd_t queue_server = -1;
int nr_of_clients = 0;
int current_client_add = 1;
int current_client = 0;


int get_sender_client(int id) {
    for(int i = 0; i < current_client; i++) {
        if (all_clients_id[i] == id)
            return i;
    }
    return -1;
}

void stop_f(char *m_mgs) {
    int id = atoi(m_mgs);
    printf("End of client %d\n", id);
    
    
    int id_table;
    for(int i = 0; i < current_client; i++){
        if (all_clients_id[i] == id)
            id_table = i;
    }
    mq_close(all_clients_mqd[id_table]);
    free(all_clients_path[id_table]);
    for(int i = id_table; i < current_client; i++) {
        all_clients_id[i] = all_clients_id[i+1];
        all_clients_mqd[i] = all_clients_mqd[i+1];
        all_clients_path[i] = all_clients_path[i+1];
        connection_clients[i] = connection_clients[i+1];
    }
    all_clients_id[current_client-1] = -1;
    all_clients_path[current_client-1] = NULL;
    all_clients_mqd[current_client-1] = 0;
    connection_clients[current_client-1] = -1;
    current_client--;
}

void quit_f2(){
    printf("End of server\n");
    char* server_end = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    char* back = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    
    for(int i = 0; i < current_client; i++) {
        mq_send(all_clients_mqd[i], server_end, BIGGEST_MESSAGE, STOP);
    }
    while(current_client > 0) {
        unsigned int nr_a;
        if(mq_receive(queue_server, back, BIGGEST_MESSAGE, &nr_a) >= 0) {
            stop_f(back);
        }
    }
    mq_close(queue_server);
    mq_unlink(SERVER_NAME);
    exit(0);
}

void quit_f(int signal) {
    printf("End of server\n");
    char* server_end = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    char* back = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    
    for(int i = 0; i < current_client; i++) {
        mq_send(all_clients_mqd[i], server_end, BIGGEST_MESSAGE, STOP);
    }
    while(current_client > 0) {
        unsigned int nr;
        if(mq_receive(queue_server, back, BIGGEST_MESSAGE, &nr) >= 0) {
            stop_f(back);
        }
    }
    mq_close(queue_server);
    mq_unlink(SERVER_NAME);
    exit(0);
}

void init_f(char *m_msg) {
    char* new_queue = m_msg;

    all_clients_path[current_client] = (char*)calloc(FILE_L, sizeof(char));
    strcpy(all_clients_path[current_client], m_msg);
    all_clients_mqd[current_client] = mq_open(new_queue, O_WRONLY, 0666);

    all_clients_id[current_client] = current_client_add++;
    connection_clients[current_client] = -1;
    
    
    
    current_client++;
    printf("Initialize new client: %d\n", all_clients_id[current_client-1]);

    char* to_client = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    sprintf(to_client, "%d", all_clients_id[current_client-1]);
    mq_send(all_clients_mqd[current_client-1], to_client, BIGGEST_MESSAGE, INIT);


                
}

void list_f(char *m_msg) {
    
    int id_sender = atoi(m_msg);
    // int client_sender = get_sender_client(id_sender);

    char* to_client = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    strcpy(to_client, "");
    
    int counter = 0;
    for(int i = 0; i < current_client; i++) {
        if(all_clients_id[i] != id_sender) {
            sprintf(to_client+ strlen(to_client), "%d - status %d\n", all_clients_id[i],
            connection_clients[i] == -1);
            counter++;
        }
        
    }
    if (counter == 0)
        sprintf(to_client, "Lack of available clients\n");
    int to_who = -1;
    for(int i = 0; i < current_client; i++) {
        if (all_clients_id[i] == id_sender)
            to_who = i;
    }    
    printf("Send list of clients to client %d\n", atoi(m_msg));
    mq_send(all_clients_mqd[to_who], to_client, BIGGEST_MESSAGE, LIST);
}

void connect_f(char *m_msg) {
    
    int id_client = atoi(strtok(m_msg, " "));
    int from = get_sender_client(id_client);
    int id_to_connected = atoi(strtok(NULL, " " ));
    int to = get_sender_client(id_to_connected);
    
    connection_clients[from] = all_clients_id[to];
    connection_clients[to] = all_clients_id[from];
    printf("Connect client %d with %d\n", all_clients_id[from], all_clients_id[to]);

    char* to_clients = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));;
    char* to_clients2 = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    
    sprintf(to_clients, "%s", all_clients_path[to]);
    
    mq_send(all_clients_mqd[from], to_clients, BIGGEST_MESSAGE, CONNECT);
    
    sprintf(to_clients2, "%d %s", all_clients_id[from], all_clients_path[from]);
    mq_send(all_clients_mqd[to], to_clients2, BIGGEST_MESSAGE, CONNECT);
}

void disconnect_f(char *m_msg) {
    
    int sender = atoi(m_msg);

    int from = get_sender_client(sender);
    int to = get_sender_client(connection_clients[from]);
    printf("Disconnecting %d and %d.\n", all_clients_id[from], all_clients_id[to]);
    connection_clients[from] = -1;
    connection_clients[to] = -1;

    char *to_client = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));;
    mq_send(all_clients_mqd[to], to_client, BIGGEST_MESSAGE, DISCONNECT);
}


int main(){
    printf("Start of server\n");
    
    struct mq_attr attr_server;
    attr_server.mq_maxmsg = MAX_COM;
    attr_server.mq_msgsize = BIGGEST_MESSAGE;
    if((queue_server = mq_open(SERVER_NAME, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr_server)) < 0) {
        printf("ERROR - %s", strerror(errno));
        mq_unlink(SERVER_NAME);
        exit(EXIT_FAILURE);
    }
    
    
    signal(SIGINT, quit_f);
    //atexit(quit_f2);
    unsigned int sig_type;
    char* m_msg = (char*)calloc(BIGGEST_MESSAGE, sizeof(char));
    while(1) {
        if(mq_receive(queue_server, m_msg, BIGGEST_MESSAGE, &sig_type) < 0) {
            printf("Cant get message - %s\n", strerror(errno));
            mq_unlink(SERVER_NAME);
            exit(EXIT_FAILURE);
        }
        //printf("%s\n", m_msg);
        switch(sig_type) {
            case INIT:
                init_f(m_msg);
                break;
            case LIST:
                list_f(m_msg);
                break;
            case CONNECT:
                connect_f(m_msg);
                break;
            case DISCONNECT:
                disconnect_f(m_msg);
                break;
            case STOP:
                stop_f(m_msg);
                break;  
        }
        
    }
}
