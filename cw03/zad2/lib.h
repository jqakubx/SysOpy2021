#ifndef lib_h
#define lib_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    int size;
    char** lines;
} merged_block; 

typedef struct {
    int size;
    int curr_size;
    merged_block** blocks;
} main_table;

typedef struct {
    char* first_file;
    char* second_file;
    int size;
} pair_fails;

// typedef struct {
//     pair_fails** list_sequence;
// } sequence;

main_table* create_table(int size);
pair_fails* read_file(char* first_file_path, char* second_file_path);
void merge_files_sequence(main_table* table, char** files);
int merge_pair(main_table* table, char* file_first, char* file_second);
int count_lines(char *file);
void show_table(main_table* table);
void delete_block(main_table* table, int index);
void delete_row(main_table* table, int block_index, int line_index);
void clean(main_table* table);

#endif