#include<errno.h>
#include<stdlib.h>
#include<stdio.h>

#include<time.h>
#include <sys/times.h>
#include<unistd.h>

double time_difference(clock_t time1, clock_t time2) {
    return (double)(time2-time1) / sysconf(_SC_CLK_TCK);
}

void print_time(clock_t start, clock_t end, struct tms time_start, struct tms time_end) {
    FILE *plik = fopen("pomiar_zad_3.txt", "a");
    fprintf(plik, "\nFunkcje z biblioteki C\n");
    fprintf(plik, "\t*Real time: %lfs\n", time_difference(start, end));
    fclose(plik);
}

int readline(char* buf, int sz, FILE *file, off_t *offset) {
    int nchr = 0;
    int idx = 0;
    char *p = NULL;
    if((nchr = fseek(file, *offset, 0)) == 0)
        nchr = fread(buf, sizeof(char), sz, file);

    if (nchr == 0) return -1;

    p = buf;
    while (idx < nchr && *p != '\n') p++, idx++;
    *p = 0;
    if (idx == nchr) {
        *offset += nchr;

        return nchr < sz ? nchr: 0;
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
    
    FILE *we1 = fopen(argv[1], "r");
    FILE *wy2 = fopen("a2.txt", "w");
    FILE *wy3 = fopen("b2.txt", "w");
    FILE *wy4 = fopen("c2.txt", "w");

    if (we1 == NULL || wy2 == NULL || wy3 == NULL || wy4 == NULL) {
        perror("Błąd oczytu plików");
        exit(-10);
    }
    char line[256];
    off_t offset = 0;
    int number;
    int even = 0;
    int len;
    while((len = readline(line, 256, we1, &offset)) != -1) {
        number = atoi(line);
        if(number % 2 == 0)
            even++;
        if(divideBy7Or0(number)) {
            fwrite(line, 1, len, wy3);
            fwrite("\n" ,1, 1, wy3);
        }
        if(isTwoPower(number)) {
            fwrite(line, 1, len, wy4);
            fwrite("\n" ,1, 1, wy4);
        }
    }
    const char *message = "Liczb parzystych jest ";
    char nr[20];
    sprintf(nr, "%d", even);
    fwrite(message, 1, 22, wy2);
    for(int i = 0; i < 20 && nr[i] != '\0'; i++)
        fwrite(&nr[i], 1, 1, wy2);

    fclose(we1);
    fclose(wy2);
    fclose(wy3);
    fclose(wy4);
    
    clock_t time_end = times(&tms_end);
    print_time(time_start, time_end, tms_start, tms_end);
    return 0;
}
