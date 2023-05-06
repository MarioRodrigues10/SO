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

int bounce(pid_t progpid, char * status, int len){   
    char fifo[] = "tmp/fifo_";
    char pid[4096];

    sprintf(pid, "%d", progpid);
    strcat(fifo, pid);
    int fd = open(fifo, O_WRONLY);

    write(fd, status, len);

    close(fd);
}

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
            program tracer = parse_string(buffer2);
            if (tracer.status == 0){

                char filename[4096];
                char buffer3[4096];
                if (tracer.running == 1){ // guarda e faz contas
                    program before;

                    char tmpbuffer[4096], saved[4096];

                    int fd = open("tmp/running", O_RDONLY);
                    int read_temp;
                    read_temp = read(fd, &tmpbuffer, 4096);
                    tmpbuffer[read_temp] = '\0';
                    memcpy(saved, tmpbuffer, read_temp);
                    saved[read_temp] = '\0';

                    char *token;

                    token = strtok(tmpbuffer, "\n");
                    
                    char pid[1000];
                    snprintf(pid, sizeof(pid),"%d",tracer.pid);

                    while(token != NULL){

                        char *ctoken;
                        ctoken = strsep(&token, " ");                        

                        if(strcmp(ctoken, pid) == 0){
                            int l = 0;
                            while(ctoken != NULL){
                                if (l == 0){
                                    before.pid = atoi(ctoken); 
                                }else if(l == 1){
                                    before.sec = atol(ctoken);
                                } else if(l == 2){
                                    before.ms = atol(ctoken);
                                } else if(l== 3){
                                    before.program = ctoken;
                                } else{
                                    char temp[10] = " ";
                                    strcat(temp, ctoken);
                                    strcat(before.program, temp);
                                }
                                ctoken = strsep(&token, " ");
                                l++;
                            }

                        }


                        token = strtok(NULL, "\n");
                    }

                    long ms = ((tracer.sec - before.sec)*1000) + ((tracer.ms - before.ms)/1000);

                    char status[100];
                    int len = sprintf(status, "Ended in %ld ms\n", ms);
                    bounce(tracer.pid, status, len);

                    char del[100];
                    int deltam = sprintf(del, "%d %ld %ld %s\n", before.pid, before.sec, before.ms, before.program);

                    removeSubstring(saved, del);

                    int fdr = open("tmp/running", O_WRONLY | O_TRUNC, 0666);
                    int w_temp = write(fdr, saved, sizeof(char) * strlen(saved));
                    if(w_temp == -1){
                        perror("Error updating tmp/running file");
                    }

                    close(fdr);
                    close(fd);

                    len = sprintf(filename, "PIDS/PID-%d", tracer.pid);
                    FILE *file = fopen(filename, "a");
                    if (file == NULL)
                    {
                        write(1, "Error opening file!\n", strlen("Error opening file!\n"));
                        return -1;
                    }
                    len = sprintf(buffer3, "%d %ld %s\n", tracer.pid, ms, tracer.program);
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
                    len = sprintf(buffer3, "%d %ld %ld %s\n", tracer.pid, tracer.sec, tracer.ms, tracer.program);
                    fwrite(buffer3, sizeof(char), len, file);
                    fclose(file);

                }
            } else{ //ve o ficheiro temp aka status
                char tmpbuffer[4096];
                
                int fd = open("tmp/running", O_RDONLY);
                int read_temp = read(fd, &tmpbuffer, 4096);
                if (read_temp != -1 && read_temp != 0){

                    char status[1];
                    status[0] = '\0';
                    tmpbuffer[read_temp] = '\0';

                    char *token;

                    token = strtok(tmpbuffer, "\n");
                    
                    program current;
                    char c[100];
                    int len = 0;
                    long ms;

                    while(token != NULL){

                        char *ctoken;
                        ctoken = strsep(&token, " ");                        

                        int l = 0;
                        while(ctoken != NULL){
                            if (l == 0){
                                current.pid = atoi(ctoken); 
                            }else if(l == 1){
                                current.sec = atol(ctoken);
                            } else if(l == 2){
                                current.ms = atol(ctoken);
                            } else if(l== 3){
                                current.program = ctoken;
                            } else{
                                char temp[10] = " ";
                                strcat(temp, ctoken);
                                strcat(current.program, temp);
                            }
                            ctoken = strsep(&token, " ");
                            l++;
                        }
                        ms = ((tracer.sec - current.sec)*1000) + ((tracer.ms - current.ms)/1000);
                        len += sprintf(c,"%d %s %ld ms\n",current.pid, current.program, ms);
                        strcat(status, c);
                        token = strtok(NULL, "\n");
                    }
                    status[len] = '\0';
                    bounce(tracer.pid,status, len);

                }else{
                   bounce(tracer.pid,"Não há programas a executar no momento\n", strlen("Não há programas a executar no momento\n"));     
                }
                    

            }

        }
        printf("after while");
        close(input);
    }

    unlink("tmp/running");
    unlink("tmp/main_fifo");

    return 0;
}
