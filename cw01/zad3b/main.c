#include "lib.h"
#include <sys/times.h>
#include<time.h>
#include<unistd.h>

double time_difference(clock_t time1, clock_t time2) {
    return (double)(time2-time1) / CLOCKS_PER_SEC;
}

double real_time_difference(clock_t time1, clock_t time2) {
    return (double) (time2-time1) / sysconf(_SC_CLK_TCK);
}

void print_time(clock_t start, clock_t end, struct tms time_start, struct tms time_end) {
    printf("\t*Real time: %lfs\n", real_time_difference(start, end));
    printf("\t*User time: %lfs\n", time_difference(time_start.tms_utime, time_end.tms_utime));
    printf("\t*System time: %lfs\n", time_difference(time_start.tms_stime, time_end.tms_stime));
}


int main(int argc, char **argv) {

    main_table* table = NULL;
    struct tms tms_start;
    struct tms tms_end;

    clock_t time_start = times(&tms_start);
    
    time_start = times(&tms_start);

    if (argc < 2) {
        printf("Too small amount of arguments.");
        return 1;
    }
    for(int i = 1; i < argc; i++) {
        char* option = argv[i];
        if(strcmp(option, "create_table") == 0) {
            int size1 = atoi(argv[++i]);
            table = create_table(size1);
        }

        else if (strcmp(option, "merge_files") == 0) {
            merge_files_sequence(table, argv + i + 1);
            i += 1;
        }

        else if (strcmp(option, "remove_block") == 0) {
            int block = atoi(argv[++i]);
            delete_block(table, block);
        }

        else if (strcmp(option, "remove_row") == 0) {
            int block = atoi(argv[++i]);
            int row = atoi(argv[++i]); 
            delete_row(table, block, row);
        }
    }

    clock_t time_end = times(&tms_end);
    print_time(time_start, time_end, tms_start, tms_end);
    //show_table(table);
    clean(table);
}