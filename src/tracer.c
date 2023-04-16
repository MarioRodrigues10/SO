#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "lib.h"

int main(int argc,char * argv[]){

    int fd = open("main_fifo",O_WRONLY);

    printf("depois open main_fifo\n");

    char buffer[4096];
    sprintf(buffer, "#%d#%d#%d#%ld#",getpid(), 0, 0, time(NULL)); // pid, running, status, time, argv
    int k = 0;
    if(argv[1][0] == '-'){
        strcat(buffer, argv[2]);
    }else{
        strcat(buffer, argv[1]);
    }

    strcat(buffer,"#");
    write(fd,buffer,strlen(buffer)); //sent all the info

    printf("depois main_fifo\n");

    close(fd);

    return 0;
}