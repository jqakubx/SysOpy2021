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
int random_between(int x1, int x2) {
    return rand() % (x2 - x1 + 1) + x1;
}

int get_pizza() {
    
    sembuf* reserve = (sembuf*) calloc(2, sizeof(sembuf));
    reserve[0].sem_num = 0;
    reserve[0].sem_op = 0;
    reserve[0].sem_flg = 0;

    reserve[1].sem_num = 0;
    reserve[1].sem_op = 1;
    reserve[1].sem_flg = 0;

    semop(shared_semaphore_table, reserve, 2);

    if(semctl(shared_semaphore_table, 3, GETVAL, NULL) == 0) {
        sembuf* err = (sembuf*) calloc(1, sizeof(sembuf));
        err[0].sem_num = 0;
        err[0].sem_op = -1;
        err[0].sem_flg = 0;
        semop(shared_semaphore_table, err, 1);
        return -1;
    }

    table_pizza *b_pizza = shmat(shared_memory_table, NULL, 0);

    union semun args;
    args.val = semctl(shared_semaphore_table, 3, GETVAL, NULL) -1;
    int index = semctl(shared_semaphore_table, 2, GETVAL, NULL);
    index += 1;
    if(index == SIZE_TABLE)
        index = 0;
    
    int type_pizza = b_pizza->pizzas[index];
    b_pizza->pizzas[index] = -1;
    semctl(shared_semaphore_table, 3, SETVAL, args);
    printf("(%d %ld) Pobieram pizze: %d Liczba pizz na stole: %d\n", getpid(), time(NULL),
        type_pizza, semctl(shared_semaphore_table, 3, GETVAL, NULL));
    shmdt(b_pizza);
    
    union semun args2;
    args2.val = index;
    semctl(shared_semaphore_table, 2, SETVAL, args2);

    sembuf* ret = (sembuf*) calloc(1, sizeof(sembuf));
    ret[0].sem_num = 0;
    ret[0].sem_op = -1;
    ret[0].sem_flg = 0;
    
    semop(shared_semaphore_table, ret, 1);

    int sleep_time = random_between(4, 5);
    sleep(sleep_time);
    return type_pizza;
}

void deliever_pizza(int type_pizza) {
    printf("(%d %ld) Dostarczam pizze: %d\n", getpid(), time(NULL),
        type_pizza);
    int sleep_time = random_between(4, 5);
    sleep(sleep_time);
}

int main() {
    
    srand(time(NULL) ^ (getpid()<<16));

    key_t semaphore_key_table = ftok(getenv("HOME"), 2);
    // 0 - is modified (0 - no, 1 - yes)
    // 1 - last add index
    // 2 - last get index
    // 3 - pizzas in table
    shared_semaphore_table = semget(semaphore_key_table, 0, 0);
    key_t memory_key_table = ftok(getenv("HOME"), 3);
    shared_memory_table = shmget(memory_key_table, 0, 0);

    while(1) {
        int type_pizza = -1;
        while(type_pizza == -1) {
            type_pizza = get_pizza();
            if(type_pizza == -1) {
                usleep(100);
                type_pizza = get_pizza();
            }
        }
        deliever_pizza(type_pizza);
    }
    return 0;
}