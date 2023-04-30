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

// int bounce()
// {
//     char fifo[] = "tmp/fifo_";
//     char pid[4096];

//     sprintf(pid, "%d", getpid());
//     strcat(fifo, pid);

//     int res = mkfifo(fifo, 0666);
//     if (res == -1)
//     {
//         perror("error creating bounce fifo");
//     }
//     int read_bytes;
//     char buffer[4096];
//     int fstatus = open(fifo, O_RDONLY);

//     while ((read_bytes = read(fstatus, buffer, 4096)) > 0)
//     {
//         printf("read %d bytes", read_bytes);
//         write(1, buffer, read_bytes);
//     }
//     close(fstatus);
//     unlink(fifo);
//     return 0;
// }


int main(int argc, char *argv[])
{

    int fd = open("tmp/main_fifo", O_WRONLY);



    int k = 0, status;

    if (argv[2][0] == '-') k = 3;
    else k = 2;


    if (strcmp(argv[0], "execute")) status = 0;
    else if (strcmp(argv[0], "status"))
    {
        status = 1;
        //bounce();
        return 0;
    }
    else
    {
        write(1, "Invalid command!\n", strlen("Invalid command!\n"));
        close(fd);
        return -1;
    }

    struct timeval start;
    gettimeofday(&start, NULL);

    program tracer;
    tracer.pid = getpid();
    tracer.running = 0;
    tracer.status = status;
    tracer.program = argv[k];

    char linha[100];
    int tam = snprintf(linha, sizeof(linha), "Running PID: %d\n", tracer.pid);
    write(1, linha, tam);

    char *input_line = (char*) malloc(strlen(argv[k]) + 1); 
    strcpy(input_line, argv[k]); 

    char **strings = parse(tracer.program);
    int tamanho = 0; while (strings[tamanho] != NULL) tamanho++;
    
    struct timeval stop;
    if (tamanho > 1) stop = execOperation(strings[2], strings[0], strings[1]);
    else stop = execOperation(strings[1], strings[0], NULL);
    double time = (stop.tv_sec - start.tv_sec) * 1000 + (stop.tv_usec - start.tv_usec) / 1000.0;

    tam = snprintf(linha, sizeof(linha), "#%d#%d#%d#%f#%s#", getpid(), 0, status, time, input_line);
    write(fd, linha, tam);
    close(fd);

    tam = sprintf(linha, "Ended in %f ms!\n", time);
    write(1, linha, tam);

    return 0;
}