// ./sender pid ilosc_sygnalow tryb


#define _XOPEN_SOURCE 500
#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>
const int KILL = 1;
const int SIGQUEUE = 2;
const int SIGRT = 3;
int end = 0;
int SIGNAL_SEND;
int SIGNAL_SEND_END;
int SIGNAL_REACHED = 0;
int type;
int catcher_id;
int signals;
void handle1(int sign, siginfo_t *info, void *x)
{
    SIGNAL_REACHED++;
    if(type == SIGQUEUE)
        {
            printf("Sender reached %d signals. Catcher send %d signals\n", SIGNAL_REACHED, info->si_value.sival_int);
        }
}
void handle2(int sign, siginfo_t *info, void *x)
{
    printf("Sender got %d signals but should got %d\n", SIGNAL_REACHED, signals);
    end = 1;
}

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        perror("Too little arguments.");
        exit(-1);
    }

    catcher_id = atoi(argv[1]);
    signals = atoi(argv[2]);
    
    if(strcmp("kill", argv[3]) == 0)
    {
        
        type = KILL;
        SIGNAL_SEND = SIGUSR1;
        SIGNAL_SEND_END = SIGUSR2;
    } 
    else if (strcmp("queue", argv[3]) == 0)
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

    if (type == KILL)
    {
        for (int i = 0; i < signals; i++)
        {
            kill(catcher_id, SIGNAL_SEND);
        }
        kill(catcher_id, SIGNAL_SEND_END);
    } else if (type == SIGRT)
    {
        for (int i = 0; i < signals; i++)
        {
            kill(catcher_id, SIGNAL_SEND);
        }
        kill(catcher_id, SIGNAL_SEND_END);
    }
    else
    {
        union sigval sig;
        sig.sival_int = 0;
        for(int i = 0; i < signals; i++)
        {
            sigqueue(catcher_id, SIGNAL_SEND, sig);
        }
        sigqueue(catcher_id, SIGNAL_SEND_END, sig);
    }
    printf("PID: %d\n", getpid());
    while(!end)
    {
        
    }
    return 0;

}
