#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>
#include<sys/types.h>
#include<stdlib.h>
#include<sys/wait.h>

void handle_signal(int signal) {
    printf("Signal handled\n");
}

int main(int argc, char** argv) {
    if(argc < 3) {
        perror("There have to be at least 3 arguments\n");
    }

    if(strcmp(argv[1], "ignore") == 0) 
    {
        signal(SIGUSR1, SIG_IGN);
    }
    else if (strcmp(argv[1], "handler") == 0)
    {
        signal(SIGUSR1, handle_signal);
    }
    else if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0)
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &mask, NULL);
    }

    raise(SIGUSR1);

    sigset_t mask;
    if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0)
    {
        
        sigpending(&mask);
        if(sigismember(&mask, SIGUSR1))
        {
            printf("Signal pending %d\n", SIGUSR1);
        }
        else
        {
            printf("Not signal pending\n");
        }
    }

    if(strcmp(argv[2], "fork") == 0)
    {
        pid_t child = fork();

        if(child == 0)
        {
            if(strcmp(argv[1], "pending") != 0) {
                raise(SIGUSR1);
            }
            if(strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0)
            {
                sigpending(&mask);
                if(sigismember(&mask, SIGUSR1))
                {
                    printf("Child signal pending %d\n", SIGUSR1);
                }
                else
                {
                    printf("Child not signal pending\n");
                }
            }
            if(strcmp(argv[1], "handler") != 0)
            {
                signal(SIGUSR1, handle_signal);
            }
        }
    }
    else if(strcmp(argv[2], "exec") == 0)
    {
        execl("./exec", "./exec", argv[1], NULL);
    }
    while (wait(NULL) > 0) {

    }

}
