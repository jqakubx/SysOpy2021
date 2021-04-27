#ifndef CHATCONFIG_H
#define CHATCONFIG_H
#include<sys/msg.h>
#include<sys/types.h>
#include<mqueue.h>

#define MAX_CLIENTS_NR 10
#define BIGGEST_MESSAGE 100
#define ID_OF_SERVER 1
#define FILE_L 10
#define MAX_COM 10

typedef struct {
    long m_type;
    char m_message[BIGGEST_MESSAGE];
} message;

typedef enum message_type {
    STOP = 1, DISCONNECT = 2, LIST = 3, INIT = 4, CONNECT = 5
} message_type;

typedef struct {
    int client_id;
    int client_queue;
    int connection;
} client;

const char* SERVER_NAME = "/SERV";


#endif