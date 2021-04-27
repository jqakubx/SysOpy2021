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
    FILE *plik = fopen("pomiar_zad_4.txt", "a");
    fprintf(plik, "\nFunkcje z biblioteki C\n");
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
    
    FILE *we1 = fopen(argv[1], "r");
    FILE *wy1 = fopen("output2.txt", "w");


    if (we1 == NULL || wy1 == NULL) {
        perror("Błąd oczytu plików");
        exit(-10);
    }
    
    char* text1 = argv[2];
    char* text2 = argv[3];
    char textFind[1024];
    int sizeFile = fseek(we1, 0, 2);
    sizeFile = ftell(we1);
    int len;
    fseek(we1, 0, 0);
    int sizeText1 = 0, sizeText2 = 0;
    for(int i = 0; text1[i] != '\0'; i++)
        sizeText1++;
    for(int i = 0; text2[i] != '\0'; i++)
        sizeText2++;
    for(int i = 0; i < sizeFile;) {
        len = fread(textFind, 1, sizeText1, we1);
        if (len == sizeText1) {
            if (sameText(textFind, text1)) {
                fwrite(text2, 1, sizeText2, wy1);
                i += sizeText1;
            }
            else {
                fwrite(&textFind[0], 1, 1, wy1);
                i += 1;
                fseek(we1, i, 0);
            }
        }
        else if (len > 0){
            fwrite(textFind, 1, len, wy1);
            i=sizeFile;
        }
        else
            i = sizeFile;
    }
    fclose(we1);
    fclose(wy1);
    
    clock_t time_end = times(&tms_end);
    print_time(time_start, time_end, tms_start, tms_end);
    return 0;
}
