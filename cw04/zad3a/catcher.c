// ./catcher tryb
#define _XOPEN_SOURCE 500
#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>
int SIGNAL_SEND;
int SIGNAL_SEND_END;
int SIGNAL_REACHED = 0;
const int KILL = 1;
const int SIGQUEUE = 2;
const int SIGRT = 3;
int type;
int send = 0;
void handle1(int signal)
{
    SIGNAL_REACHED++;
}
void handle2(int signal, siginfo_t *info, void *x)
{
    send = info->si_pid;
}

int main(int argc, char *argv[])
{
    
    if (argc < 2)
    {
        perror("To little arguments");
        exit(-1);
    }

    if(strcmp("kill", argv[1]) == 0)
    {
        type = KILL;
        SIGNAL_SEND = SIGUSR1;
        SIGNAL_SEND_END = SIGUSR2;
    } 
    else if (strcmp("queue", argv[1]) == 0)
    {
        type = SIGQUEUE;
        SIGNAL_SEND = SIGUSR1;
        SIGNAL_SEND_END = SIGUSR2;
    } 
    else
    {
        type = SIGRT;
        SIGNAL_SEND = SIGRTMIN;
        SIGNAL_SEND_END = SIGRTMAX;
    }

    sigset_t mask_block;
    sigfillset(&mask_block);
    sigdelset(&mask_block, SIGNAL_SEND);
    sigdelset(&mask_block, SIGNAL_SEND_END);
    sigprocmask(SIG_BLOCK, &mask_block, NULL);

    struct sigaction handler1;
    handler1.sa_flags = 0;
    handler1.sa_handler = handle1;
    sigemptyset(&handler1.sa_mask);
    struct sigaction handler2;
    handler2.sa_flags = SA_SIGINFO;
    handler2.sa_sigaction = handle2;
    sigemptyset(&handler2.sa_mask);
    sigaddset(&handler1.sa_mask, SIGNAL_SEND);
    sigaddset(&handler2.sa_mask, SIGNAL_SEND_END);
    sigaction(SIGNAL_SEND, &handler1, NULL);
    sigaction(SIGNAL_SEND_END, &handler2, NULL);
    printf("PID: %d\n", getpid());
    while(!send)
    {}
    if (type == KILL)
    {
        for (int i = 0; i < SIGNAL_REACHED; i++)
        {
            kill(send, SIGNAL_SEND);
        }
        kill(send, SIGNAL_SEND_END);
    } else if (type == SIGRT)
    {
        for (int i = 0; i < SIGNAL_REACHED; i++)
        {
            kill(send, SIGNAL_SEND);
        }
        kill(send, SIGNAL_SEND_END);
    }
    else
    {
        union sigval sig;
        
        for(int i = 0; i < SIGNAL_REACHED; i++)
        {
            sig.sival_int = i+1;
            sigqueue(send, SIGNAL_SEND, sig);
        }
        sigqueue(send, SIGNAL_SEND_END, sig);
    }
    printf("Catcher reached %d signals\n", SIGNAL_REACHED);
    return 0;
}
