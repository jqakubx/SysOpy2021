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
    FILE *plik = fopen("pomiar_zad_5.txt", "a");
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

int main(int argc, char** argv) {
    struct tms tms_start;
    struct tms tms_end;

    clock_t time_start = times(&tms_start);


    if (argc != 3) {
        perror("Zła liczba argumentów");
        exit(-10);
    }
    
    FILE *we1 = fopen(argv[1], "r");
    FILE *wy1 = fopen(argv[2], "w");


    if (we1 == NULL || wy1 == NULL) {
        perror("Błąd oczytu plików");
        exit(-10);
    }
    
    char line[256];
    off_t offset = 0;
    int len;
    int size;
    while((len = readline(line, 256, we1, &offset)) != -1) {
        if (len <= 50) {
            fwrite(line, 1, len, wy1);
            fwrite("\n", 1, 1, wy1);
        }
        else {
            size = 0;
            for(int i = 0; line[i] != '\0'; i++)
                size++;
            for(int i = 0; (i+1)*50 < size; i++) {
                fwrite((line+(i*50)), 1, 50, wy1);
                fwrite("\n", 1, 1, wy1);
            }
            fwrite((line + (50*(size/50))), 1, size%50, wy1);
            fwrite("\n", 1, 1, wy1);
        }
    }
    fclose(we1);
    fclose(wy1);
    
    clock_t time_end = times(&tms_end);
    print_time(time_start, time_end, tms_start, tms_end);
    return 0;
}
