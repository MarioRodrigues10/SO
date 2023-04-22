#include <time.h>
#include <sys/types.h>

typedef struct{
    int running; // 0 for running, 1 for done
    int status; // 0 for client exec, 1 for status request
    pid_t pid; // process pid
    char* program; //program to execute
    time_t time; //timestamp
} program;

typedef int file_d; // file descriptor
