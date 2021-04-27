#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define MAX_LENGTH 256
#define MAX_INGREDIENTS 15
#define MAX_ARGUMENTS 15
#define MAX_ARGUMENTS_ONE 100
int count_ing(char* line)
{
    int j = 9;
    int res = 0;
    char sign;
    while(line[j] != '\0')
    {
        sign = line[j];
        res *= 10;
        res += sign - '0';
        j++;
    }
    return (res-1);
}
void del_spaces(char* line)
{
    while(isspace(line[0]))
    {
        for(int i = 0; i < strlen(line) - 1; i++)
        {
            line[i] = line[i+1];
            line[i+1] = ' ';
        }
    }
    int j = strlen(line)-1;
    while(isspace(line[j]))
    {
        line[j] = '\0';
        j--;
    }
}
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Too low arguments");
        return 1;
    }

    FILE* commands = fopen(argv[1], "r");
    char line[MAX_LENGTH + 1];
    char line2[MAX_LENGTH + 1];
    char* ingredients[MAX_INGREDIENTS][MAX_ARGUMENTS];
    char* arguments_one_line[MAX_ARGUMENTS_ONE];
    for(int i = 0; i < MAX_INGREDIENTS; i++)
    {
        for(int j = 0; j < MAX_ARGUMENTS; j++)
            ingredients[i][j] = NULL;
    }
    int ingredient_cur = 0;
    int arg_cur = 0;
    char* tmp_line;
    char* tmp_line2;
    while (fgets(line, MAX_LENGTH, commands) != NULL)
    {
        
        arg_cur = 0;
        
        if(strcmp(line, "\n") == 0)
            break;
        
        del_spaces(line);
        tmp_line = strtok(line, "=");
        tmp_line = strtok(NULL, "=");
        tmp_line2 = strtok(tmp_line, "|");
        
        while(tmp_line2 != NULL)
        {
            del_spaces(tmp_line2);
            //strcpy(ingredients[ingredient_cur][arg_cur++], tmp_line2);
            ingredients[ingredient_cur][arg_cur] = malloc(strlen(tmp_line2)+1);
            strcpy(ingredients[ingredient_cur][arg_cur], tmp_line2);
            tmp_line2 = strtok(NULL, "|");
            arg_cur++;
        }
        
        ingredient_cur++;
    }
    int ingredient_list[MAX_INGREDIENTS];
    int cur_ingr;
    int arg_c;
    char* tmp_line3;
    int st;
    while (fgets(line2, MAX_LENGTH, commands) != NULL)
    {
        
        arg_c = 0;
        cur_ingr = 0;
        tmp_line3 = strtok(line2, "|");
        del_spaces(tmp_line3);
        while(tmp_line3 != NULL)
        {
            del_spaces(tmp_line3);
            ingredient_list[cur_ingr++] = count_ing(tmp_line3);
            tmp_line3 = strtok(NULL, "|");
        }  
        
        for(int i = 0; i < MAX_ARGUMENTS_ONE; i++)
            arguments_one_line[i] = NULL;
        for(int i = 0, k = 0; i < cur_ingr; i++)
        {
            
            k = 0;
            while(ingredients[ingredient_list[i]][k] != NULL)
            {
                arguments_one_line[arg_c] = ingredients[ingredient_list[i]][k];
                arg_c++;
                k++;
            }
        }
        
        char* functions[MAX_INGREDIENTS][MAX_ARGUMENTS];
        for(int i = 0; i < MAX_INGREDIENTS; i++)
        {
            for(int j = 0; j < MAX_ARGUMENTS; j++)
                functions[i][j] = NULL;
        }
        char *line_new;
        char *tmp;
        for(int z = 0, x; z < arg_c; z++)
        {
            x = 0;
            line_new = arguments_one_line[z];
            tmp = strtok(line_new, " ");
            functions[z][x] = tmp;
            while (tmp != NULL)
            {
                x++;
                tmp = strtok(NULL, " ");
                functions[z][x] = tmp;
            }
        }

        int current[2], previous[2];
        pid_t waitpid;
        for(int z = 0; z < arg_c; z++)
        {
            int g = 0;
            while(functions[z][g] != NULL)
                printf("%s ", functions[z][g++]);
            
            pipe(current);
            pid_t child = fork();
            if(child == 0)
            {
                
                if (z != 0)
                {
                    dup2(previous[0], STDIN_FILENO);
                    close(previous[1]);
                }
                if (z + 1 < arg_c)
                    dup2(current[1], STDOUT_FILENO);
                execvp(functions[z][0], functions[z]);
                exit(0);
            }
            close(current[1]);
            previous[0] = current[0];
            previous[1] = current[1];
        }
        printf("\n");
        while((waitpid = wait(&st)) > 0);
        
        
    }
    for(int i = 0; i < ingredient_cur; i++)
    {
        int j = 0;
        while(ingredients[i][j] != NULL)
            free(ingredients[i][j++]);
    }
    fclose(commands);
}

