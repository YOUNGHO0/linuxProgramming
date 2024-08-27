//
// Created by lee on 2024. 8. 27..
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

int main( int argc , char * argv[]){


    if(argc != 2){
        fprintf(stderr, "usage %s dirname \n", argv[0] );
        exit(1);
    }

    struct stat fbuf;
    if(stat(argv[1] , &fbuf)== -1){
        perror("stat");
        exit(1);
    }

    printf("inode :  %d , size : %d \n", fbuf.st_ino, fbuf.st_size);


    DIR * dp;
    if(S_ISDIR(fbuf.st_mode)){

        printf("Directory\n");

        dp = opendir(argv[1]);
        if(dp == NULL){
            perror("opendir");
            exit(1);
        }


        struct dirent* dirp;

        while(1){
            dirp = readdir(dp);
            if(dirp ==NULL)
                break;

            printf("innode : %d, filename : %s \n", dirp->d_ino, dirp->d_name );
        }

    }else if(S_ISREG(fbuf.st_mode)){
        printf("Regular file\n");

    }
}