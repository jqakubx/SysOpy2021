#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
int indexes[100];
char signs[100][10000];
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Too few arguments");
        return 1;
    }
    const char* sign = argv[3];
    char *fifo_file_s = argv[1];
    char *source_file = argv[2];
    int N = atoi(sign);
    FILE *fifo_file = fopen(fifo_file_s, "r");
    FILE *data_file = fopen(source_file, "w");
    char data_from_file[N];
    char data_with_index[N+1];
    int row_nr;
    char* sign2 = malloc(1);
    int max_id = 1;
    while(fgets(data_with_index, N+2, fifo_file) != NULL)
    {
        printf("SAVE %s\n", data_with_index);
        sign2[0] = data_with_index[0];
        row_nr = atoi(sign2);
        if(row_nr > max_id)
            max_id = row_nr;
        for(int i = 1; i < (N+1); i++)
            data_from_file[i-1] = data_with_index[i];
        for(int i = 0; i < N; i++)
        {
            signs[row_nr-1][indexes[row_nr-1]] = data_from_file[i];
            indexes[row_nr-1] += 1;
        }
        //fwrite(data_from_file, 1, N, data_file);
        //fflush(data_file);
    }
    for(int i = 0; i < max_id; i++)
    {
        fwrite(signs[i], 1, indexes[i], data_file);
        fwrite("\n", 1, 1, data_file);
        fflush(data_file);
    }
    free(sign2);
    fclose(data_file);
    fclose(fifo_file);
    
}