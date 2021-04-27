#include<errno.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>

#include<time.h>
#include <sys/times.h>


double time_difference(clock_t time1, clock_t time2) {
    return (double)(time2-time1) / sysconf(_SC_CLK_TCK);
}   

void print_time(clock_t start, clock_t end, struct tms time_start, struct tms time_end) {
    FILE *plik = fopen("pomiar_zad_5.txt", "a");
    fprintf(plik, "Funkcje systemowe\n");
    fprintf(plik, "\t*Real time: %lfs\n", time_difference(start, end));
    fclose(plik);
}

ssize_t readline(char* buf, size_t sz, int fd, off_t *offset) {
    ssize_t nchr = 0;
    ssize_t idx = 0;
    char *p = NULL;

    if((nchr = lseek(fd, *offset, SEEK_SET)) != -1)
        nchr = read(fd, buf, sz);

    if (nchr == 0) return -1;
    
    p = buf;
    while (idx < nchr && *p != '\n') p++, idx++;
    *p = 0;
    if (idx == nchr) {
        *offset += nchr;

        return nchr < (ssize_t)sz ? nchr: 0;
    }

    *offset += idx + 1;
    return idx;
}

int main(int argc, char** argv) {

    struct tms tms_start;
    struct tms tms_end;

    clock_t time_start = times(&tms_start);
    
    
    if (argc != 3) {
        perror("Zła liczba argumentów");
        exit(-10);
    }
    int we1 = open(argv[1], O_RDONLY);
    int wy1 = open(argv[2], O_WRONLY | O_APPEND | O_CREAT, 0666);

    if (we1 == -1 || wy1 == -1) {
        perror("Błąd oczytu plików");
        exit(-10);
    }


    char line[256];
    off_t offset = 0;
    ssize_t len = 0;
    int size;
    while((len = readline(line, 256, we1, &offset))  != -1) {

        if (len <= 50) {
            write(wy1, line, len);
            write(wy1, "\n", 1);
        }
        else {
            size = 0;
            for(int i = 0; line[i] != '\0'; i++)
                size++;
            for(int i = 0; (i+1)*50 < size; i++) {
                write(wy1, (line+(i*50)), 50);
                write(wy1, "\n", 1);
            }
            write(wy1, (line + (50*(size/50))), size%50);
            write(wy1, "\n", 1);
        }
          
    }


    close(we1);
    close(wy1);

    clock_t time_end = times(&tms_end);
    print_time(time_start, time_end, tms_start, tms_end);
    return 0;
}
