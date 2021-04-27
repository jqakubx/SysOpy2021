#include "lib.h"
#include <sys/times.h>
#include<time.h>
#include<unistd.h>
#include<dlfcn.h>

double time_difference(clock_t time1, clock_t time2) {
    return (double)(time2-time1) / CLOCKS_PER_SEC;
}

double real_time_difference(clock_t time1, clock_t time2) {
    return (double) (time2-time1) / sysconf(_SC_CLK_TCK);
}

void print_time(clock_t start, clock_t end, struct tms time_start, struct tms time_end) {
    printf("\t*Real time: %f\n", real_time_difference(start, end));
    printf("\t*User time: %f\n", time_difference(time_start.tms_utime, time_end.tms_utime));
    printf("\t*System time: %f\n", time_difference(time_start.tms_stime, time_end.tms_stime));
}

int main(int argc, char **argv) {

    void* dl_handle = dlopen("./lib.so", RTLD_LAZY);
    if(!dl_handle) {
        printf("Cannto load dynamic lib");
        return 1;
    }

    struct main_table* (*dl_create_table)(int);
    dl_create_table = (struct main_table* (*)(int))dlsym(dl_handle, "create_table");
    
    void (*dl_merge_files_sequecne)(struct main_table*, char**);
    dl_merge_files_sequecne = (void (*)(struct main_table*, char**))dlsym(dl_handle, "merge_files_sequence");

    void (*dl_delete_block)(struct main_table*, int);
    dl_delete_block = (void (*)(struct main_table*, int))dlsym(dl_handle, "delete_block");

    void (*dl_delete_row)(struct main_table*, int, int);
    dl_delete_row = (void (*)(struct main_table*, int, int))dlsym(dl_handle, "delete_row");

    void (*dl_clean)(struct main_table*);
    dl_clean = (void(*)(struct main_table*))dlsym(dl_handle, "clean");

    // void (*dl_show)(struct main_table*);
    // dl_show = (void(*)(struct main_table*))dlsym(dl_handle, "show");

    struct main_table* table = NULL;
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
            table = dl_create_table(size1);
        }

        else if (strcmp(option, "merge_files") == 0) {
            dl_merge_files_sequecne(table, argv + i + 1);
            i += 1;
        }

        else if (strcmp(option, "remove_block") == 0) {
            int block = atoi(argv[++i]);
            dl_delete_block(table, block);
        }

        else if (strcmp(option, "remove_row") == 0) {
            int block = atoi(argv[++i]);
            int row = atoi(argv[++i]); 
            dl_delete_row(table, block, row);
        }
    }

    clock_t time_end = times(&tms_end);
    print_time(time_start, time_end, tms_start, tms_end);
    //dl_show_table(table);
    dl_clean(table);
}