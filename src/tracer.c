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

int bounce()
{
    char fifo[] = "tmp/fifo_";
    char pid[4096];

    sprintf(pid, "%d", getpid());
    strcat(fifo, pid);

    int res = mkfifo(fifo, 0666);
    if (res == -1)
    {
     perror("error creating bounce fifo");
    }
    int read_bytes;
    char buffer[4096];
    int fstatus = open(fifo, O_RDONLY);

    while ((read_bytes = read(fstatus, buffer, 4096)) > 0)
    {
     printf("read %d bytes", read_bytes);
     write(1, buffer, read_bytes);
    }
    close(fstatus);
    unlink(fifo);
    return 0;
}

int mysystem(char in[]){
    char *comando;
    comando =strdup(in);
    int i = 0;
    char *string;
    char *exec_args[20];
    string = strsep(&comando, " ");
    while(string!=NULL){
        exec_args[i]=string;
        string=strsep(&comando," ");
        i++;
    }

    exec_args[i]=NULL;

    int exect_ret;
    int status;

    if(fork()==0){
        exect_ret = execvp(exec_args[0],exec_args);

        perror("reached return");

        _exit(exect_ret);
    }

    pid_t pid = wait(&status);
    if(WIFEXITED(status)){
        return WEXITSTATUS(status);
    }else{
        printf("filho %d deu rip\n", pid);
        return -1;
    }
    return 0;
}


int main(int argc, char *argv[])
{

    int fd = open("tmp/main_fifo", O_WRONLY);
    int status;
    // status
    if (strcmp(argv[1], "status") == 0)
    {
        status = 1;
        bounce();
        return 0;
    } //normal exec
    else if((strcmp(argv[1], "execute") == 0) && (strcmp(argv[2], "-u") == 0))
    {   
        program tracer;
        tracer.pid = getpid();
        tracer.program = argv[3];

        char linha[100];
        int tam = snprintf(linha, sizeof(linha), "Running PID: %d\n", tracer.pid);
        write(1, linha, tam);

        struct timeval time;
        gettimeofday(&time, NULL);
        tracer.ms = time.tv_usec;
        tracer.sec = time.tv_sec;
        tam = snprintf(linha, sizeof(linha), "#%d#%d#%d#%ld#%ld#%s#", tracer.pid, 0, 0, tracer.sec, tracer.ms ,tracer.program);
        write(fd, linha, tam);

        mysystem(tracer.program);

        gettimeofday(&time, NULL);
        tracer.ms = time.tv_usec;
        tracer.sec = time.tv_sec;
        tam = snprintf(linha, sizeof(linha), "#%d#%d#%d#%ld#%ld#%s#", tracer.pid, 1, 0, tracer.sec, tracer.ms ,tracer.program);
        printf("%s\n",linha);
        fflush(stdout);
        write(fd, linha, tam);

        close(fd);

        tam = sprintf(linha, "Ended!\n");
        write(1, linha, tam);

        return 0;
    } //pipelines
    else if ((strcmp(argv[1], "execute") == 0) && (strcmp(argv[2], "-p") == 0))
    {
        printf("Entrei abc\n");
        return 0;
    }
    else {
        write(1, "Invalid command!\n", strlen("Invalid command!\n"));
        close(fd);
        return -1;
    
    }
}