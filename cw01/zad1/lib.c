#include"lib.h"
main_table* create_table(int size) {
    main_table* table = calloc(sizeof(main_table*), 1);
    table->blocks = calloc(sizeof(merged_block*), size);
    table->size = size;
    table->curr_size = 0;
    return table;
}

int count_lines(char *file_z) {
    FILE *file = fopen(file_z, "r");
    if (file == NULL ) return 0;
    char b[250];
    int lines = 0;
    while (fgets(b, sizeof(b), file) != NULL && b[0] != '\0') lines++;
    fclose(file);
    return lines;
}

pair_fails* read_file(char* first_file_path, char* second_file_path) {
    if(first_file_path == NULL || second_file_path == NULL) return NULL;

    FILE* filestream1 = fopen(first_file_path,"r");
    FILE* filestream2 = fopen(second_file_path,"r");
    if(filestream1 == NULL || filestream2 == NULL) return NULL;

    pair_fails* pair_f = calloc(sizeof(pair_fails), 1);
    pair_f->size = count_lines(first_file_path) + count_lines(second_file_path);
    

    fseek(filestream1, 0, SEEK_END);
    long fsize = ftell(filestream1);
    rewind(filestream1);
    pair_f->first_file = calloc(fsize + 1, 1);
    fread(pair_f->first_file, 1, fsize, filestream1);

    fseek(filestream2, 0, SEEK_END);
    fsize = ftell(filestream2);
    rewind(filestream2);
    pair_f->second_file = calloc(fsize + 1, 1);
    fread(pair_f->second_file, 1, fsize, filestream2);
    
    fclose(filestream1);
    fclose(filestream2);
    return pair_f;
}

int merge_pair(main_table* table, char* file_first, char* file_second) {
    pair_fails* pair_files_v = calloc(sizeof(pair_fails), 1);
    pair_files_v = read_file(file_first, file_second);
    FILE *file = tmpfile();
    char *cp1 = strdup(pair_files_v->first_file);
    char *cp2 = strdup(pair_files_v->second_file);
    char* string1 = strtok_r(cp1, "\n", &cp1);
    char* string2 = strtok_r(cp2, "\n", &cp2);
    while(string1 != NULL && string2 != NULL) {
        fputs(string1, file);
        fputs("\n", file);
        fputs(string2, file);
        fputs("\n", file);
        string1 = strtok_r(NULL, "\n", &cp1);
        string2 = strtok_r(NULL, "\n", &cp2);
    }
    rewind(file);
    table->blocks[table->curr_size] = calloc(sizeof(merged_block*), 1);
    table->blocks[table->curr_size]->size = pair_files_v->size;
    table->blocks[table->curr_size]->lines = (char**)calloc(pair_files_v->size, sizeof(char*));
    size_t buff = 0;
    for(int i = 0;i < pair_files_v->size; i++) {
        getline(&(table->blocks[table->curr_size]->lines[i]), &buff, file);
        buff = 0;
    }
    free(pair_files_v);
    
    table->curr_size = table->curr_size+1;
    return table->curr_size-1;
}

void merge_files_sequence(main_table* table, char** files) {
    //int size = table->size;
    
    static char* files_s[2];
    //for(int i = 0; i < size; i++) {
        files_s[0] = strtok(files[0], ":");
        files_s[1] = strtok(NULL, "");
        merge_pair(table, files_s[0], files_s[1]);
    //}
}

void show_table(main_table* table) {
    int size = table->size;
    for (int i = 0; i < size; i++) {
        if (table->blocks[i] != NULL) {
            for(int j = 0; j < table->blocks[i]->size; j++) {
                if (table->blocks[i]->lines[j] != NULL)
                    printf("%s", table->blocks[i]->lines[j]);
            }
        }
    }
}

void delete_block(main_table *table, int index) {
    if (table->blocks[index] != NULL) {
        free(table->blocks[index]->lines);
        table->blocks[index]->lines = NULL;
        free(table->blocks[index]);
        table->blocks[index] = NULL;
        
        // for(int i = index; i < table->curr_size-1; i++) {
        //     table->blocks[i] = table->blocks[i+1];
        // }
        // table->curr_size-=1;
    }
}

void delete_row(main_table *table, int b_index, int l_index) {
    if (table->blocks[b_index] != NULL) {
        if(table->blocks[b_index]->lines[l_index] != NULL) {
            free(table->blocks[b_index]->lines[l_index]);
            table->blocks[b_index]->lines[l_index] = NULL;
            // for(int i = l_index; i < table->blocks[b_index]->size-1; i++) {
            // table->blocks[b_index]->lines[i] = table->blocks[b_index]->lines[i+1];
            // }
            // table->blocks[b_index]-> size -=1;


        }
    }
}

void clean(main_table* table) {
    for (int i = 0; i < table->size; i++) {
        if(table->blocks[i] == NULL) continue;
        free(table->blocks[i]->lines);
        free(table->blocks[i]);
    } 
    free(table->blocks);
    free(table);
}

//test
int main(int argc, char** argv) {
    main_table* table = create_table(2);
    merge_files_sequence(table, argv+1);
    merge_files_sequence(table, argv+2);
    delete_block(table, 1);
    delete_row(table, 0, 2);
    show_table(table);
    //printf("%s", table->blocks[1]->lines[5]);
    clean(table);
}
