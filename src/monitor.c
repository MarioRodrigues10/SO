#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include "lib.h"
#include <stdlib.h>

void cut(char new[], char string[], int size){
    for (int l = 0;l<size;l++){
        new[l] = string[l];  
        new[l+1] = '\0';                  
    }
}

//criar um fifo com o pid do cliente e comunicar de volta
int bounce(program tracer){
    char fifo[] = "fifo_";
    char pid[4096];
    char buffer[4096];
    sprintf(pid,"%d",tracer.pid);
    strcat(fifo,pid);

    int fd = open(fifo,O_WRONLY);

    write(fd,"status bounce",strlen("status bounce"));
    close(fd);
    
    return 0;
}

int main(int argc,char * argv[]){

    char buffer[4096];
    int read_bytes = 1;

    int res = mkfifo("main_fifo",0666);
    if(res==-1){
        char error[] = "Error creating the main fifo\n";
        write(1, error, strlen(error));
    }
    write(1, "main fifo created\n", strlen("main fifo created\n"));

    int fd = open("main_fifo",O_RDONLY);
    int nullv = open("main_fifo",O_WRONLY);
    while(read_bytes>0){
        read_bytes=read(fd,buffer,4096);
        char temp[strlen(buffer)-6];
        program tracer;
        int k = 0;
        int j = 0;
        //client info to struct
        for (int i =0;i<strlen(buffer) && k != 6;i++){
            if (buffer[i] == '#'){
                if (k ==1){
                    //pid
                    char pid[j];
                    cut(pid,temp,j);
                    tracer.pid = atoi(pid);
                } if (k ==2){
                    //run
                    char run[j];
                    cut(run,temp,j);
                    tracer.running = atoi(run);
                } if (k== 3){
                    //status
                    char stat[j];
                    cut(stat,temp,j);
                    tracer.status = atoi(stat);
                } if (k==4){
                    //time
                    char time[j];
                    cut(time,temp,j);
                    tracer.time = atoi(time);
                } if (k == 5){
                    char args[j];
                    cut(args,temp,j);
                    tracer.program=args;
                }
                k++;
                for(int l =0;l<j;l++){
                    temp[l] = ' ';
                }
                j = 0;
            } else{
                temp[j] = buffer[i];
                j++;
            }
        }
        // if status = 0, guardar status else dar status
        if (tracer.status == 0){

        } else if (tracer.status == 1){
            int rbounce = bounce(tracer);
        }
    }
    printf("after while");     
    close(fd);


    
    unlink("main_fifo");

    return 0;
}
