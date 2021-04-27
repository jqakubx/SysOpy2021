#ifndef PIZZERIACONFIG_H
#define PIZZERIACONFIG_H

#include<stdio.h>
#include<stdlib.h>
#include<sys/sem.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>
#include<time.h>

#define SIZE_TABLE 5
#define SIZE_BAKE 5
#define CHIEF_NUMBER 7
#define DELIEVER_NUMBER 7

typedef struct sembuf sembuf;

typedef struct table_pizza {
    int pizzas[SIZE_TABLE];
} table_pizza;

typedef struct bake_pizza {
    int pizzas[SIZE_TABLE];
} bake_pizza;

#define rand_sleep ((rand() % (1000-100+1) + 100) * 1000)

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};
#endif //PIZZERIACONFIG_H