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
    FILE *plik = fopen("pomiar_zad_2.txt", "a");
    fprintf(plik, "Funkcje systemowe\n");
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
    int we1;
    int we2;
    if(argc < 3) {
        printf("Wprowadź nazwy plików\n");
        scanf("%s", name1);
        scanf("%s", name2);
        we1 = open(name1, O_RDONLY);
        we2 = open(name2, O_RDONLY);
    }
    else {
        we1 = open(argv[1], O_RDONLY);
        we2 = open(argv[2], O_RDONLY);
    }
    if (we1 == -1 || we2 == -1) {
        perror("Błąd oczytu plików");
        exit(-10);
    }
    int numb1;
    int numb2;
    while((numb1=read(we1,&sign1,1)) > 0 && (numb2=read(we2,&sign2,1)) > 0) {
        while (sign1 != '\n' && numb1 > 0) {
            printf("%c", sign1);
            numb1 = read(we1, &sign1, 1);
        }
        printf("\n");
        while (sign2 != '\n' && numb2 > 0) {
            printf("%c", sign2);
            numb2 = read(we2, &sign2, 1);
        }
        printf("\n");
    }
    if(numb1 > 0)
        printf("%c", sign1);
    if(numb2 > 0 && sign2!= '\n')
        printf("%c", sign2);
    while(numb1 > 0) {
        
        numb1 = read(we1, &sign1, 1);
        while(numb1 > 0 && sign1 != '\n') {
            printf("%c", sign1);
            numb1 = read(we1, &sign1, 1);
        }
        if(numb1 > 0)
            printf("\n");
        
    }
    while(numb2 > 0) {
        
        numb2 = read(we2, &sign2, 1);
        while(numb2 > 0 && sign2 != '\n') {
            printf("%c", sign2);
            numb2 = read(we2, &sign2, 1);
        }
        if(numb2 > 0)
            printf("\n");
    }
    close(we1);
    close(we2);

    clock_t time_end = times(&tms_end);
    print_time(time_start, time_end, tms_start, tms_end);
    return 0;
}
