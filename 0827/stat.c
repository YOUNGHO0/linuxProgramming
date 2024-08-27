//
// Created by lee on 2024. 8. 27..
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>


int main( int argc , char * argv[]){


    if(argc != 2){
        fprintf(stderr, "usage %s filename \n", argv[0] );
        exit(1);
    }

    struct stat fbuf;
    if(stat(argv[1] , &fbuf)== -1){
        perror("stat");
        exit(1);
    }

    printf("inode :  %d , size : %d \n", fbuf.st_ino, fbuf.st_size);


    if(S_ISDIR(fbuf.st_mode)){

        printf("Directory\n");

    }else if(S_ISREG(fbuf.st_mode)){
        printf("Regular file\n");

    }
}