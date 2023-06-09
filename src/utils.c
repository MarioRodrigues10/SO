#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "lib.h"
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>

/*
 * Function:  parse_string
 * --------------------
 *  parses a string into an array of strings
 *
 *  string: string to be parsed
 *
 *  returns: program struct with the parsed data
 */
program parse_string(char *string)
{
    char *token = strtok(string, "#");
    char **strings = NULL;
    int i = 0, num_strings = 1;
    while (token != NULL)
    {
        strings = (char **)realloc(strings, num_strings * sizeof(char *));
        strings[i] = token;
        token = strtok(NULL, "#");
        i++, num_strings++;
    }
    program tracer;
    tracer.pid = atoi(strings[0]);
    tracer.running = atoi(strings[1]);
    tracer.status = atoi(strings[2]);
    tracer.sec = atol(strings[3]);
    tracer.ms = atol(strings[4]);
    tracer.program = strings[5];

    return tracer;
}

char **parse(char *string)
{
    char *token = strtok(string, " ");
    char **strings = NULL;
    int i = 0, num_strings = 1;
    while (token != NULL)
    {
        strings = (char **)realloc(strings, num_strings * sizeof(char *));
        strings[i] = token;
        token = strtok(NULL, " ");
        i++, num_strings++;
    }
    return strings;
}

int parser(char **exec_args, char *str){
    char *comando;
    comando =strdup(str);
    int i = 0;
    char *string;
    string = strsep(&comando, " ");
    if(strcmp("", string) == 0){
        string = strsep(&comando, " ");
    }
    while(string!=NULL){
        if(strcmp("", string) != 0){
            exec_args[i]=string;
        }
        string=strsep(&comando," ");
        i++;
    }
    return i;
}

int pipeline(char **comandos, int nc)
{

    int pipes[nc-1][2];

    for(int i = 0; i<nc; i++){

        char *exec_args[20];
        for(int j = 0; j<20;j++){
            exec_args[j] = NULL;
        }
        parser(exec_args, comandos[i]);

        if (i==0){
            pipe(pipes[i]);
            if(fork() == 0){
                close(pipes[i][0]);
                dup2(pipes[i][1],1);
                close(pipes[i][1]);
                execvp(exec_args[0], exec_args);
            } else{
                close(pipes[i][1]);
            }
        }else if (i==nc-1){
            if(fork() == 0){
                dup2(pipes[i-1][0],0);
                close(pipes[i-1][0]);
                execvp(exec_args[0], exec_args);
            } else{
                close(pipes[i-1][1]);
            }
        }else{
            pipe(pipes[i]);
            if(fork() == 0){
                close(pipes[i][0]);
                dup2(pipes[i-1][0],0);
                close(pipes[i][0]);
                dup2(pipes[i][1],1);
                close(pipes[i][1]);
                execvp(exec_args[0], exec_args);
            } else{
                close(pipes[i-1][0]);
                close(pipes[i][1]);
            }
        }
    }
    int w;
    for(int i =0; i<nc;i++){
        wait(&w); 
    }

    return 0;

}

int parse_pipeline(char **args, char *cmd_str)
{

    char *token;

    token = strtok(cmd_str, "|");
    int i = 0;

    while(token != NULL){

        args[i] = token;

        token = strtok(NULL, "|");
        i++;
    }
    
    return i;
}

void removeSubstring(char *s, const char * toremove) {
    int i, j, k;
    int n = strlen(s);
    int m = strlen(toremove);

    for (i = 0; i < n - m + 1; i++) {
        for (j = 0, k = i; j < m && s[k] == toremove[j]; j++, k++);
        if (j == m) {
            for (k = i + m; k <= n; k++) {
                s[k - m] = s[k];
            }
            n -= m;
            i--;
        }
    }
}


