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
    FILE *plik = fopen("pomiar_zad_3.txt", "a");
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

int divideBy7Or0(int number) {
    if(number < 10) return 0;
    int numb2 = number%100;
    numb2 /= 10;
    if (numb2 == 0 || numb2 == 7)
        return 1;
    return 0;
}

int isTwoPower(int number) {
    for (int i = 1; i < number/2; i++)
        if (i * i == number)
            return 1;
    return 0;
}
int main(int argc, char** argv) {

    struct tms tms_start;
    struct tms tms_end;

    clock_t time_start = times(&tms_start);
    
    
    if (argc != 2) {
        perror("Zła liczba argumentów");
        exit(-10);
    }
    int we1 = open(argv[1], O_RDONLY);
    int wy1 = open("a.txt", O_WRONLY | O_APPEND | O_CREAT, 0666);
    int wy2 = open("b.txt", O_WRONLY | O_APPEND | O_CREAT, 0666);
    int wy3 = open("c.txt", O_WRONLY | O_APPEND | O_CREAT, 0666);
    if (we1 == -1 || wy1 == -1 || wy2 == -1 || wy3 == -1) {
        perror("Błąd oczytu plików");
        exit(-10);
    }
    char line[256];
    off_t offset = 0;
    ssize_t len = 0;
    int number;
    int even = 0;
    while((len = readline(line, 256, we1, &offset)) != -1) {
        
        number = atoi(line);
        if(number % 2 == 0)
            even++;
        if(divideBy7Or0(number)) {
            write(wy2, line, len);
            write(wy2,"\n", 1);
        }
        if(isTwoPower(number)) {
            write(wy3, line, len);
            write(wy3, "\n", 1);
        }
    }
    const char *message = "Liczb parzystych jest ";
    char nr[20];
    sprintf(nr, "%d", even);
    write(wy1, message, 22);
    for(int i = 0; i < 20 && nr[i] != '\0'; i++)
        write(wy1, &nr[i], 1);

    close(we1);
    close(wy1);
    close(wy2);
    close(wy3);
    clock_t time_end = times(&tms_end);
    print_time(time_start, time_end, tms_start, tms_end);
    return 0;
}
