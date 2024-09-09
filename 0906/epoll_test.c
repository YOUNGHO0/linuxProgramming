//
// Created by lee on 2024. 9. 6..
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <string.h>

int main(){


    int efpds;
    struct epoll_event ev;
    efps = epoll_create(1);
    if(efpds == -1){
        perror("epoll_error");
    }
    ev.events = EPOLLIN;
    ev.data.fd = 0;

    if(epoll_ctl( efps,EPOLL_CTL_ADD, 0,&ev )){
        perror("epoll_ctl");
        exit(1);
    }


    while(1){
        memset(buf,0,sizeof(buf));

        n = epoll_wait(efpds,evlist,3 -1);
        printf("n = %d\n", n);

        for(int i =0; i<n; i++){
            if(evlist[i].event == EPOLLIN){
                fgets()
            }
        }
    }

}

