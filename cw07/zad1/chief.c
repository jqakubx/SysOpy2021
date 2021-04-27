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
int random_between(int x1, int x2) {
    return rand() % (x2 - x1 + 1) + x1;
}

int made_pizza() {
    int pizza_type = random_between(0, 9);
    printf("(%d %ld) Przygotowuje pizze: %d\n",
             getpid(), time(NULL), pizza_type);
    
    int made_pizza = random_between(1, 2);
    sleep(made_pizza);
    
    return pizza_type;
}

int add_pizza_to_bake(int pizza_type) {
    sembuf* reserve = (sembuf*) calloc(2, sizeof(sembuf));
    reserve[0].sem_num = 0;
    reserve[0].sem_op = 0;
    reserve[0].sem_flg = 0;

    reserve[1].sem_num = 0;
    reserve[1].sem_op = 1;
    reserve[1].sem_flg = 0;
    
    semop(shared_semaphore_bake, reserve, 2);
    
    if(semctl(shared_semaphore_bake, 2, GETVAL, NULL) == SIZE_BAKE) {
        sembuf* err = (sembuf*) calloc(1, sizeof(sembuf));
        err[0].sem_num = 0;
        err[0].sem_op = -1;
        err[0].sem_flg = 0;
        semop(shared_semaphore_bake, err, 1);
        return -1;
    }


    int index = -1;
    bake_pizza *b_pizza = shmat(shared_memory_bake, NULL, 0);
    
    int last = semctl(shared_semaphore_bake, 1, GETVAL, NULL);
    
    for(int i = last + 1; i < SIZE_BAKE; i++) {
        if (b_pizza->pizzas[i] == -1)
            index = i;
    }
    if(index == -1) {
        for(int i = 0; i <= last; i++) {
            if (b_pizza->pizzas[i] == -1)
                index = i;
        }
    }
    
    union semun args;
    args.val = semctl(shared_semaphore_bake, 2, GETVAL, NULL) + 1;
    b_pizza->pizzas[index] = pizza_type;
    semctl(shared_semaphore_bake, 2, SETVAL, args);
    printf("(%d %ld) Dodałem pizze: %d. Liczba pizz w piecu: %d\n",
            getpid(), time(NULL), pizza_type, semctl(shared_semaphore_bake, 2, GETVAL, NULL));
    shmdt(b_pizza);
    
    union semun args2;
    args2.val = index;
    semctl(shared_semaphore_bake, 1, SETVAL, args2);
    sembuf* ret = (sembuf*) calloc(1, sizeof(sembuf));
    ret[0].sem_num = 0;
    ret[0].sem_op = -1;
    ret[0].sem_flg = 0;
    semop(shared_semaphore_bake, ret, 1);
    return index;
    
}

void get_pizza(int index) {
    int sleep_time = random_between(4, 5);
    sleep(sleep_time);
    sembuf* reserve = (sembuf*) calloc(2, sizeof(sembuf));
    reserve[0].sem_num = 0;
    reserve[0].sem_op = 0;
    reserve[0].sem_flg = 0;

    reserve[1].sem_num = 0;
    reserve[1].sem_op = 1;
    reserve[1].sem_flg = 0;

    semop(shared_semaphore_bake, reserve, 2);

    bake_pizza *b_pizza = shmat(shared_memory_bake, NULL, 0);

    union semun args;
    args.val = semctl(shared_semaphore_bake, 2, GETVAL, NULL) -1;
    b_pizza->pizzas[index] = -1;
    semctl(shared_semaphore_bake, 2, SETVAL, args);
    shmdt(b_pizza);

    sembuf* ret = (sembuf*) calloc(1, sizeof(sembuf));
    ret[0].sem_num = 0;
    ret[0].sem_op = -1;
    ret[0].sem_flg = 0;
    
    semop(shared_semaphore_bake, ret, 1);
}

int add_pizza_to_table(int pizza_type) {
    sembuf* reserve = (sembuf*) calloc(2, sizeof(sembuf));
    reserve[0].sem_num = 0;
    reserve[0].sem_op = 0;
    reserve[0].sem_flg = 0;

    reserve[1].sem_num = 0;
    reserve[1].sem_op = 1;
    reserve[1].sem_flg = 0;
    
    semop(shared_semaphore_table, reserve, 2);
    
    
    if(semctl(shared_semaphore_table, 3, GETVAL, NULL) == SIZE_TABLE) {
        
        sembuf* err = (sembuf*) calloc(1, sizeof(sembuf));
        err[0].sem_num = 0;
        err[0].sem_op = -1;
        err[0].sem_flg = 0;
        semop(shared_semaphore_table, err, 1);
        return -1;
    }
    
    table_pizza *t_pizza = shmat(shared_memory_table, NULL, 0);
    
    int last = semctl(shared_semaphore_table, 1, GETVAL, NULL);
    
    union semun args;
    args.val = semctl(shared_semaphore_table, 3, GETVAL, NULL) + 1;
    last = last + 1;
    if (last >= SIZE_TABLE)
        last = 0;
    t_pizza->pizzas[last] = pizza_type;
    semctl(shared_semaphore_table, 3, SETVAL, args);

    union semun args2;
    args2.val = last;
    semctl(shared_semaphore_table, 1, SETVAL, args2);
    printf("(%d %ld) Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n",
            getpid(), time(NULL), pizza_type, semctl(shared_semaphore_bake, 2, GETVAL, NULL),
            semctl(shared_semaphore_table, 3, GETVAL, NULL));
    shmdt(t_pizza);
    
    sembuf* ret = (sembuf*) calloc(1, sizeof(sembuf));
    ret[0].sem_num = 0;
    ret[0].sem_op = -1;
    ret[0].sem_flg = 0;
    semop(shared_semaphore_table, ret, 1);
    return 1;
    
}

int main() {
    
    srand(time(NULL) ^ (getpid()<<16));
    key_t semaphore_key_bake = ftok(getenv("HOME"), 0);
    // 0 - id modified (0 - no, 1 - yes)
    // 1 - last add index
    // 2 - pizzas in bake
    shared_semaphore_bake = semget(semaphore_key_bake, 0, 0);
    key_t memory_key_bake = ftok(getenv("HOME"), 1);
    shared_memory_bake = shmget(memory_key_bake, 0, 0);

    key_t semaphore_key_table = ftok(getenv("HOME"), 2);
    // 0 - is modified (0 - no, 1 - yes)
    // 1 - last add index
    // 2 - last get index
    // 3 - pizzas in table
    shared_semaphore_table = semget(semaphore_key_table, 0, 0);
    key_t memory_key_table = ftok(getenv("HOME"), 3);
    shared_memory_table = shmget(memory_key_table, 0, 0);

    while(1) {
        int type_pizza = made_pizza();
        int index = -1;
        while(index == -1) {
            index = add_pizza_to_bake(type_pizza);
            if(index == -1) {
                usleep(100);
                index = add_pizza_to_bake(type_pizza);
            }
        }
        get_pizza(index);
        int index2 = -1;
        while(index2 == -1) {
            index2 = add_pizza_to_table(type_pizza);

            if(index2 == -1) {
                usleep(100);
                
                index2 = add_pizza_to_table(type_pizza);
            }
        }
    }
    return 0;
}