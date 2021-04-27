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
    FILE *plik = fopen("pomiar_zad_1.txt", "a");
    fprintf(plik, "\nFunkcje z biblioteki C\n");
    fprintf(plik, "\t*Real time: %lfs\n", time_difference(start, end));
    fclose(plik);
}

int main(int argc, char** argv) {
    struct tms tms_start;
    struct tms tms_end;

    clock_t time_start = times(&tms_start);

    char sign1;
    char sign2;
    if (argc > 3) {
        perror("Zła liczba argumentów");
        exit(-10);
    }
    char name1[30];
    char name2[30];
    FILE *we1;
    FILE *we2;
    if(argc < 3) {
        printf("Wprowadź nazwy plików\n");
        scanf("%s", name1);
        scanf("%s", name2);
        we1 = fopen(name1, "r");
        we2 = fopen(name2, "r");
    }
    else {
        we1 = fopen(argv[1], "r");
        we2 = fopen(argv[2], "r");
    }
    if (we1 == NULL || we2 == NULL) {
        perror("Błąd oczytu plików");
        exit(-10);
    }
    int firstNull = 0;
    while(fscanf(we1, "%c", &sign1) == 1 && fscanf(we2, "%c", &sign2) == 1) {
        printf("%c", sign1);
        while (fscanf(we1, "%c", &sign1) == 1 && sign1 != '\n') {
            printf("%c", sign1);
        }
        if(sign1 != '\n')
            firstNull = 1;
        printf("\n");
        printf("%c", sign2);
        while (fscanf(we2, "%c", &sign2) == 1 && sign2 != '\n') {
            printf("%c", sign2);
        }

        printf("\n");
    }
    
    if (firstNull == 0)
        printf("%c", sign1);
    while(fscanf(we1, "%c", &sign1) == 1) {
        printf("%c", sign1);
        while (fscanf(we1, "%c", &sign1) == 1 && sign1 != '\n') {
            printf("%c", sign1);
        }
        if (sign1 == '\n')
            printf("\n");
    }
    
    
    while(fscanf(we2, "%c", &sign2) == 1) {
        printf("%c", sign2);
        while (fscanf(we2, "%c", &sign2) == 1 && sign2 != '\n') {
            printf("%c", sign2);
        }
        if(sign2 == '\n')
            printf("\n");
    }
    fclose(we1);
    fclose(we2);
    clock_t time_end = times(&tms_end);
    print_time(time_start, time_end, tms_start, tms_end);
    return 0;
}
