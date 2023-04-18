#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "lib.h"
#include <sys/wait.h>

int bounce(){
    char fifo[] = "fifo_";
    char pid[4096];

    sprintf(pid,"%d",getpid());
    strcat(fifo,pid);

    int res = mkfifo(fifo,0666);
    if(res == -1){
        perror("error creating bounce fifo");
    }
    int read_bytes;
    char buffer[4096];    
    int fstatus = open(fifo,O_RDONLY);

    while((read_bytes=read(fstatus,buffer,4096))>0){
        write(1,buffer,read_bytes);
    }
    close(fstatus);
    unlink(fifo);
    return 0;
}


int exe(char comando[]){
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
    }



int main(int argc,char * argv[]){

    int fd = open("main_fifo",O_WRONLY);

    printf("depois open main_fifo\n");

    char buffer[4096];

    int k = 0;

    if(argv[2][0] == '-'){
        k = 3;
    }else{
        k = 2;
    }
    int status;
    if(strcmp(argv[1] , "execute") == 0){
        status = 0;
    } else if(strcmp(argv[1], "status") == 0){
        status = 1;
    } else{
        printf("Unknown command");
        close(fd);
        return -1;
    }
    sprintf(buffer, "#%d#%d#%d#%ld#",getpid(), 0, status, time(NULL)); // pid, running, status, time
    strcat(buffer, argv[k]);
    strcat(buffer,"#");
    write(fd,buffer,strlen(buffer)); //sent all the info

    printf("depois main_fifo\n");

    close(fd);
    if(status == 0){
        exe(argv[k]);
    }
    else if (status == 1){
        bounce();
    }

    return 0;
}