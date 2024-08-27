//
// Created by lee on 2024. 8. 27..
//
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

int main(){


    int fd;
    if(fd= open("rlock.txt", O_RDWR | O_CREAT, 0644) != -1){
        perror("open");
    }
    char buf[16];

    for(int i =0; i<10; i++){
        memset(buf,0,16);
        sprintf(buf,"%d",0);
        write(fd,buf,16);
    }
    close(fd);
    
    return 0;
}