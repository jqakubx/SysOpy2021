#include<stdio.h>

int main() {
    printf("Procese o PID nr %d wywolany z procesu o PID nr %d", (int)getpid(), (int)getppid());
}