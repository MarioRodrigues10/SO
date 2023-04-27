#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>


#include "lib.h"

//criar um fifo com o pid do cliente e comunicar de volta
int bounce(program tracer){
    char fifo[] = "tmp/fifo_";
    char pid[4096];
    sprintf(pid,"%d",tracer.pid);
    strcat(fifo,pid);

    int fd = open(fifo,O_WRONLY);

    write(fd,"status bounce",strlen("status bounce"));
    close(fd);
    
    return 0;
}

/*
    * Function:  parse_string 
    * --------------------
    *  parses a string into an array of strings
    * 
    *  string: string to be parsed
    * 
    *  returns: program struct with the parsed data
*/
program parse_string(char* string){
    char* token = strtok(string, "#");
    char** strings = NULL;
    int i=0, num_strings = 1;
    while(token != NULL){
        strings = (char**) realloc(strings, num_strings * sizeof(char*));
        strings[i] = token;
        token = strtok(NULL, "#");
        i++, num_strings++;
    }
    program tracer;
    tracer.pid = atoi(strings[0]);
    tracer.running = atoi(strings[1]);
    tracer.status = atoi(strings[2]);
    tracer.time = atoi(strings[3]);
    tracer.program = strings[4];
    
    return tracer;
}

char** parse(char* string){
    char* token = strtok(string, " ");
    char** strings = NULL;
    int i=0, num_strings = 1;
    while(token != NULL){
        strings = (char**) realloc(strings, num_strings * sizeof(char*));
        strings[i] = token;
        token = strtok(NULL, " ");
        i++, num_strings++;
    }
    return strings;
}


pid_t execOperation(char* file, char* operation, char* second_operator) {
    pid_t pid;
    if (!(pid = fork ())){
        if (second_operator != NULL) execlp(operation, operation, second_operator, file, NULL);
        else execlp(operation, operation, file, NULL);
        exit(EXIT_SUCCESS);
    }
    return pid;
}


int main(int argc,char * argv[]){

    
    int read_bytes = 1; 

    int res = mkfifo("tmp/main_fifo",0666);
    char buffer2[4096];
    memset(buffer2, 0, 4096);
    if(res==-1) write(1, "Error creating the main fifo", strlen("Error creating the main fifo\n")); 
    else{
        write(1, "Main fifo created\n", strlen("Main fifo created\n"));
        file_d input = open("tmp/main_fifo",O_RDONLY);
        file_d nullv = open("tmp/main_fifo",O_WRONLY);
        while(read_bytes > 0){

            read_bytes = read(input,buffer2,4096);
            printf("buffer: %s\n", buffer2);
            program tracer = parse_string(buffer2);
            
            char filename[4096];
            int len = sprintf(filename, "PIDS/PID-%d.txt", tracer.pid);
            FILE *file = fopen(filename, "a");
            if (file == NULL) {
                write(1, "Error opening file!\n", strlen("Error opening file!\n"));
                return 1;
            }
            char buffer3[4096];
            len = sprintf(buffer3, "PID-%d %f %s\n", tracer.pid, tracer.time, tracer.program);
            fwrite(buffer3, sizeof(char), len, file);
            fclose(file);
            
        }
        printf("after while");     
        close(input);
    }
    
    unlink("tmp/main_fifo");

    return 0;
}


