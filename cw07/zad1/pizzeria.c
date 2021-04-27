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
#include "pizzeriaconfig.h"

int shared_semaphore_table;
int shared_memory_table;
int shared_semaphore_bake;
int shared_memory_bake;
pid_t workers[CHIEF_NUMBER];


void clear_handler(int signum) {
    printf("End of pizzeria program\n");

    for(int i = 0; i < CHIEF_NUMBER + DELIEVER_NUMBER; i++)
    kill(workers[i],SIGINT);
    shmctl(shared_memory_table, IPC_RMID, NULL);
    semctl(shared_semaphore_table, 0, IPC_RMID, NULL);
    shmctl(shared_memory_bake, IPC_RMID, NULL);
    semctl(shared_semaphore_bake, 0, IPC_RMID, NULL);
    exit(0);
}

void exec_workers() {
    for (int i = 0; i < CHIEF_NUMBER; i++) {
        pid_t chief = fork();
        if (chief == 0) {
            execlp("./chief", "chief", NULL);
        }
        workers[i] = chief;
    }
    for(int i = CHIEF_NUMBER; i < CHIEF_NUMBER + DELIEVER_NUMBER; i++) {
        pid_t deliever = fork();
        if (deliever == 0) {
            execlp("./deliever", "deliever", NULL);
        }
        workers[i] = deliever;
    }
    
    for(int i = 0; i < CHIEF_NUMBER + DELIEVER_NUMBER; i++) {
        wait(NULL);
    }
    
}

int main() {
    signal(SIGINT, clear_handler);

    key_t semaphore_key_bake = ftok(getenv("HOME"), 0);
    // 0 - id modified (0 - no, 1 - yes)
    // 1 - last add index
    // 2 - pizzas in bake
    shared_semaphore_bake = semget(semaphore_key_bake, 3, IPC_CREAT | 0666);
    key_t memory_key_bake = ftok(getenv("HOME"), 1);
    shared_memory_bake = shmget(memory_key_bake, sizeof(struct bake_pizza), IPC_CREAT | 0666);

    key_t semaphore_key_table = ftok(getenv("HOME"), 2);
    // 0 - is modified (0 - no, 1 - yes)
    // 1 - last add index
    // 2 - last get index
    // 3 - pizzas in table
    shared_semaphore_table = semget(semaphore_key_table, 4, IPC_CREAT | 0666);
    key_t memory_key_table = ftok(getenv("HOME"), 3);
    shared_memory_table = shmget(memory_key_table, sizeof(struct table_pizza), IPC_CREAT | 0666);
    union semun arg;
    arg.val = 0;
    
    for(int i = 0; i < 3; i++)
        semctl(shared_semaphore_bake, i, SETVAL, arg);
    for(int i = 0; i < 4; i++)
        semctl(shared_semaphore_table, i, SETVAL, arg);
    table_pizza *t_pizza = shmat(shared_memory_table, NULL, 0);
    bake_pizza *b_pizza = shmat(shared_memory_bake, NULL, 0);
    for(int i = 0; i < SIZE_BAKE; i++) {
        t_pizza->pizzas[i] = -1;
        b_pizza->pizzas[i] = -1;
    }
    shmdt(t_pizza);
    shmdt(b_pizza);
    
    exec_workers();
    clear_handler(0);
    return 0;
}