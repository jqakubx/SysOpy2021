#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>
#include<sys/types.h>
#include<stdlib.h>

int main(int argc, char **argv)
{
    sigset_t mask;
    if(strcmp(argv[1], "pending") != 0) 
    {
        raise(SIGUSR1);
    }
    if(strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0)
    {
        sigpending(&mask);
        printf("Signal pending %d\n", sigismember(&mask, SIGUSR1));
    }
    return 0;
}