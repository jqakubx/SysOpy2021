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
    FILE *plik = fopen("pomiar_zad_4.txt", "a");
    fprintf(plik, "Funkcje systemowe\n");
    fprintf(plik, "\t*Real time: %lfs\n", time_difference(start, end));
    fclose(plik);
}

int sameText(char* text1, char* text2) {
    for(int i = 0; text1[i] != '\0' && text2[i] != '\0'; i++)
        if(text1[i] != text2[i])
            return 0;
    return 1;
}


int main(int argc, char** argv) {

    struct tms tms_start;
    struct tms tms_end;

    clock_t time_start = times(&tms_start);
    
    
    if (argc != 4) {
        perror("Zła liczba argumentów");
        exit(-10);
    }
    int we1 = open(argv[1], O_RDONLY);
    int wy1 = open("output.txt", O_WRONLY | O_APPEND | O_CREAT, 0666);

    if (we1 == -1 || wy1 == -1) {
        perror("Błąd oczytu plików");
        exit(-10);
    }

    char* text1 = argv[2];
    char* text2 = argv[3];
    char textFind[1024];
    int sizeFile = lseek(we1, 0, SEEK_END);
    int len;
    lseek(we1, 0, SEEK_SET);
    int sizeText1 = 0, sizeText2 = 0;
    for(int i = 0; text1[i] != '\0'; i++)
        sizeText1++;
    for(int i = 0; text2[i] != '\0'; i++)
        sizeText2++;
    for(int i = 0; i < sizeFile;) {
        len = read(we1, textFind, sizeText1);
        if (len == sizeText1) {
            if (sameText(textFind, text1)) {
                write(wy1, text2, sizeText2);
                i += sizeText1;
            }
            else {
                write(wy1, &textFind[0], 1);
                i += 1;
                lseek(we1, i, SEEK_SET);
            }
        }
        else if (len > 0){
            write(wy1, textFind, len);
            i=sizeFile;
        }
        else
            i = sizeFile;
    }
    


    close(we1);
    close(wy1);

    clock_t time_end = times(&tms_end);
    print_time(time_start, time_end, tms_start, tms_end);
    return 0;
}
