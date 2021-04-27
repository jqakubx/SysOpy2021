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
int end = 0;
int send;
void send_signal(int end, int pid_id)
{
    union sigval sigv;
    sigv.sival_int = SIGNAL_REACHED;
    if(end == 0)
    {
        if (type == SIGQUEUE)
            sigqueue(pid_id, SIGNAL_SEND, sigv);
        else
            kill(pid_id, SIGNAL_SEND);
    }
    else
    {
        if (type == SIGQUEUE)
            sigqueue(pid_id, SIGNAL_SEND_END, sigv);
        else
            kill(pid_id, SIGNAL_SEND_END);
    }
}
void handle1(int signal, siginfo_t *info, void *x)
{
    send = info->si_pid;
    SIGNAL_REACHED++;
    send_signal(0, send);
}
void handle2(int signal, siginfo_t *info, void *x)
{
    end = 1;
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
    handler1.sa_flags = SA_SIGINFO;
    handler1.sa_sigaction = handle1;
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
    while(!end)
    {}
    
    printf("Catcher reached %d signals\n", SIGNAL_REACHED);
    send_signal(1, send);
    return 0;
}