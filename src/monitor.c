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

int main(int argc, char *argv[])
{

    int read_bytes = 1;

    int res = mkfifo("tmp/main_fifo", 0666);
    char buffer2[4096];
    memset(buffer2, 0, 4096);
    if (res == -1)
        write(1, "Error creating the main fifo", strlen("Error creating the main fifo\n"));
    else
    {
        write(1, "Main fifo created\n", strlen("Main fifo created\n"));
        file_d input = open("tmp/main_fifo", O_RDONLY);
        file_d nullv = open("tmp/main_fifo", O_WRONLY);
        while (read_bytes > 0)
        {
            read_bytes = read(input, buffer2, 4096);
            printf("buffer: %s\n", buffer2);
            program tracer = parse_string(buffer2);
            if (tracer.status == 0){

                char filename[4096];
                if (tracer.running == 1){ // guarda e faz contas
                    program before;
                    char buffer3[4096];
                    int fd = open("tmp/running", O_RDWR);

                    while(read(fd, &buffer3, strlen(buffer3))>0){ //isto ta mal, mas ler linha por linha
                        char pid[100];
                        pid[0] = '\0';
                        snprintf(pid, sizeof(pid),"%d",tracer.pid);
                        if (strstr(buffer3,pid) == 0){ //se pid na linha certa
                            printf("linha correta: %s\n",buffer3);
                            //del da linha no tmp/running
                            //calcular tempo (isso eu faço)
                            //bounce();
                        }
                        fflush(stdout);
                    }

                    close(fd);


                    //guarda o tempo final (ignora isto por enquanto)
                    int len = sprintf(filename, "PIDS/PID-%d", tracer.pid);
                    FILE *file = fopen(filename, "a");
                    if (file == NULL)
                    {
                        write(1, "Error opening file!\n", strlen("Error opening file!\n"));
                        return -1;
                    }
                    //char buffer3[4096];
                    len = sprintf(buffer3, "PID-%d %ld %ld %s\n", tracer.pid, tracer.sec, tracer.ms, tracer.program);
                    fwrite(buffer3, sizeof(char), len, file);
                    fclose(file);

                }else{ // guardar o 1º tempo

                    int len = sprintf(filename, "tmp/running");
                    FILE *file = fopen(filename, "a");
                    if (file == NULL)
                    {
                        write(1, "Error opening temp file!\n", strlen("Error opening temp file!\n"));
                        return -1;
                    }
                    char buffer3[4096];
                    len = sprintf(buffer3, "PID-%d %ld %ld %s\n", tracer.pid, tracer.sec, tracer.ms, tracer.program);
                    fwrite(buffer3, sizeof(char), len, file);
                    fclose(file);

                }
            } else{ //ve o ficheiro temp aka status

            }

        }
        printf("after while");
        close(input);
    }

    unlink("tmp/main_fifo");

    return 0;
}
