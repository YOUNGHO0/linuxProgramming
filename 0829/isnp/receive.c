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
    fd = open("mypipe", O_RDONLY);

    if( fd == -1){
        perror("open");
        exit(1);
    }
    int n ;
    char buf[] = "hello pipe";
    for(int i =0; i<5; i++){
        n =read(fd, buf,strlen(buf));
        buf[n] = '\0';
        printf("from send > %s\n", buf);
    }
    sleep(1);
    close(fd);

}
