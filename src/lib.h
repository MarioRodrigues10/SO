#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>

typedef struct{
    int running; // 0 for running, 1 for done
    int status; // 0 for client exec, 1 for status request
    pid_t pid; // process pid
    char* program; //program to execute
    long sec; //sec
    long ms; // ms sem sec
} program;

typedef int file_d; // file descriptor


program parse_string(char* string);
char **parse(char *string);
int pipeline(char **cmd, int num_cmds);
int parse_pipeline(char **args, char *cmd_str);
void removeSubstring(char *s, const char * toremove);
