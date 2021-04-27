#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<dirent.h>
#include<string.h>
#include<wait.h>
int sameText(char* text1, char* text2) {
    for(int i = 0; text1[i] != '\0' || text2[i] != '\0'; i++)
        if(text1[i] != text2[i])
            return 0;
    return 1;
}
int compareFileAndPattern(char* fileName, char* pattern) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) return 0;
    int sizeFile = fseek(file, 0, 2);
    sizeFile = ftell(file);
    fseek(file, 0, 0);
    int sizeText1 = 0;
    int len;
    
    for(int i = 0; pattern[i] != '\0'; i++)
        sizeText1++;
    char textFind[sizeText1];
    for(int i = 0; i < sizeFile;) {
        len = fread(textFind, 1, sizeText1, file);
        if (len == sizeText1) {
            if (sameText(textFind, pattern)) {
                fclose(file);
                return 1;
            }
            else {
                i += 1;
                fseek(file, i, 0);
            }
        }
        else {
            i = sizeFile;
        }
        
    }
    fclose(file);
    return 0;
}

void findFiles(const char* path, char* pattern, int depth) {
    if (depth > 0) {
        DIR* main_dir = opendir(path);
        struct dirent* file;
        pid_t child_pid;
        char new_path[FILENAME_MAX];
        if((main_dir = opendir(path))) {
            while ((file = readdir(main_dir)))
            {
                if(file->d_type == DT_DIR && !sameText(file->d_name, "..") && !sameText(file->d_name, ".")) {
                    if ((child_pid = fork()) == 0) {
                        strcpy(new_path, path);
                        strcat(new_path, "/");
                        strcat(new_path, file->d_name);
                        findFiles(new_path, pattern, depth-1);
                        memset(new_path, 0, FILENAME_MAX);
                        exit(0);
                    }
                    while (wait(NULL) > 0) {

                    }
                }
                if(file->d_type == DT_REG) {
                    strcpy(new_path, path);
                    strcat(new_path, "/");
                    strcat(new_path, file->d_name);
                    if(compareFileAndPattern(new_path, pattern)) {
                        printf("Plik %s zawiera slowo \"%s\". Pid procesu to: %d\n", new_path, pattern, (int)getpid());
                    }
                    memset(new_path, 0, FILENAME_MAX);
                }
            }
            
            closedir(main_dir);
        }
    }

    
}

int main(int argc, char** argv) {
    
    if (argc != 4) {
        perror("Zła liczba argumentów");
        exit(-1);
    }

    const char* main_dir = argv[1];
    char* pattern = argv[2];
    int depth = atoi(argv[3]);

    pid_t child_pid;
    if ((child_pid = fork()) == 0) {    
        findFiles(main_dir, pattern, depth);
        exit(0);
    }
    while (wait(NULL) > 0) {

    }
    return 0;

}