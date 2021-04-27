#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("Too few arguments");
        return 1;
    }
    
    const char* sign = argv[4];
    char *fifo_file_s = argv[1];
    char* row_nr = argv[2];
    char *source_file = argv[3];
    int N = atoi(sign);

    FILE *data_file = fopen(source_file, "r");
    int fifo_file = open(fifo_file_s, O_WRONLY);

    char data_from_file[N];
    char data_with_index[N+1];
    srand(time(NULL));
    while(fread(data_from_file, 1, N, data_file) > 0)
    {
        
        for(int i = N; i > 0; i--)
            data_with_index[i] = data_from_file[i-1];
        data_with_index[0] = row_nr[0];
        printf("READ: %s\n", data_with_index);
        write(fifo_file, data_with_index, N+1);
        sleep(rand() % 2 + 1);
    }
    fclose(data_file);
    close(fifo_file);
    return 0;
}