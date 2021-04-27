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

void do_tstp(int signum)
{
    kill(getpid(), SIGKILL);
}

int main()
{
    printf("SA_NOCLDSTOP. Process will be stoped. Press CTRL+Z\n\n");
    // this flag don't generate SIGCHLD when children stop
    struct sigaction signal_child_act;
    signal_child_act.sa_sigaction = child_handler;
    signal_child_act.sa_flags = SA_NOCLDSTOP;
    sigemptyset(&signal_child_act.sa_mask);
    sigaction(SIGCHLD, &signal_child_act, NULL);
    pid_t pid;
    
    signal(SIGTSTP, do_tstp);
    if ((pid = fork()) == 0) {
        
        kill(getpid(), SIGTSTP);
        
    }
    while (wait(NULL) > 0) {

    }
    printf("If there would be SA_SIGINFO this stop would be handled not by process with pid 0.");
    printf("\n\n");
}