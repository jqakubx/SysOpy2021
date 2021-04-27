#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
int main(int arhc, char* argv[]) {
    int process;
    process = atoi(argv[1]);
    pid_t* processess;
    processess = (pid_t*) calloc(process, sizeof(pid_t));
    for(int i = 0; i < process; i++) {
        processess[i] = fork();
        if (processess[i] == 0) {
            printf("Procese o PID nr %d wywolany z procesu o PID nr %d\n", (int)getpid(), (int)getppid());
            return EXIT_SUCCESS;
        }
    }
    while (wait(NULL) > 0) {

    }
    printf("Wywolano %d procesow", process);
    return 0;
}