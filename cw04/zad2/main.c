#include<signal.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<sys/wait.h>

void child_handler(int signal, siginfo_t *child, void *val)
{
    printf("Child option:\n");
    printf("Sending process %d\n", child->si_pid);
    printf("Signal number: %d\n", signal);
    printf("Exit code: %d\n\n", child->si_status);
}

void int_handler(int signal, siginfo_t * status, void *val)
{
    printf("Signal stop option:\n");
    printf("Sending process %d\n", status->si_pid);
    printf("Signal number: %d\n", signal);
    if(status->si_code == SI_USER)
    {
        printf("Kernel signal\n");
    } else if (status->si_code == SI_KERNEL)
    {
        printf("User signal\n");
    } else if (status->si_code == SI_QUEUE)
    {
        printf("Queue signal\n\n");
    }
    
}

void alarm_handler(int signal, siginfo_t * alarm, void *val)
{
    printf("Alarm option:\n");
    printf("Sending process %d\n", alarm->si_pid);
    printf("Signal number: %d\n", signal);
    if(alarm->si_code == SI_KERNEL) 
    {
        printf("Kernel Time is up\n");
    }
    printf("Signal value: %d\n\n", alarm->si_value.sival_int);
    
}

int main()
{
    printf("SA_SIGINFO\n");
    struct sigaction signal_child_act;
    signal_child_act.sa_sigaction = child_handler;
    signal_child_act.sa_flags = SA_SIGINFO;
    sigemptyset(&signal_child_act.sa_mask);
    sigaction(SIGCHLD, &signal_child_act, NULL);
    if (fork() == 0) {
        exit(15);
    }
    while (wait(NULL) > 0) {

    }

    struct sigaction signal_int_act;
    signal_int_act.sa_sigaction = int_handler;
    signal_int_act.sa_flags = SA_SIGINFO;
    sigemptyset(&signal_int_act.sa_mask);
    sigaction(SIGINT, &signal_int_act, NULL);
    union sigval sig;
    sigqueue(getpid(), SIGINT, sig);
    // or pause() Ctrl+C user signal

    struct sigaction signal_alarm_act;
    signal_alarm_act.sa_sigaction = alarm_handler;
    signal_alarm_act.sa_flags = SA_SIGINFO;
    sigemptyset(&signal_alarm_act.sa_mask);
    sigaction(SIGALRM, &signal_alarm_act, NULL);
    alarm(1);
    pause();

    printf("\n\n");
}