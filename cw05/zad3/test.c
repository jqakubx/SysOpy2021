#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/types.h>

int main(int argc, char*argv[])
{
    mkfifo("fifo", 0666);
    pid_t pides[6];
    
    if((pides[0] = fork()) == 0)
        execlp("./consumer", "./consumer", "fifo", "out.txt", "5", NULL);
    if((pides[1] = fork()) == 0)
    {
        execlp("./producer", "./producer", "fifo", "1", "file1.txt", "5", NULL);
    }
    if((pides[2] = fork()) == 0)
        execlp("./producer", "./producer", "fifo", "2", "file2.txt", "5", NULL);
    if((pides[3] = fork()) == 0)
        execlp("./producer", "./producer", "fifo", "3", "file3.txt", "5", NULL);
    if((pides[4] = fork()) == 0)
        execlp("./producer", "./producer", "fifo", "4", "file4.txt", "5", NULL);
    if((pides[5] = fork()) == 0)
        execlp("./producer", "./producer", "fifo", "5", "file5.txt", "5", NULL);
    
    
    while (wait(NULL) > 0)
    ;
    return 0;
}