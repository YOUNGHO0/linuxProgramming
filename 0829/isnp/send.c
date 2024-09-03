//
// Created by lee on 2024. 8. 29..
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
int main(){

    int fd;
    fd = open("mypipe", O_WRONLY);

    if( fd == -1){
        perror("open");
        exit(1);
    }
    char buf[] = "hello pipe";
    for(int i =0; i<5; i++){
        write(fd, buf,strlen(buf));
    }
    sleep(3);
    close(fd);

}
