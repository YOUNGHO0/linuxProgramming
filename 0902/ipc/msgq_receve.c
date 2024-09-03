//
// Created by lee on 2024. 8. 29..
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <string.h>
int main(){

    int msgid;
    struct msgbuf{
        long mtype;
        char mtext[1024];
    } mbuf;

    msgid = msgget(ftok(".", 10), IPC_CREAT|0644);
    if(msgid == -1){
        perror("msgget");
        exit(1);
    }
    if(msgrcv(msgid, &mbuf,sizeof(mbuf.mtext),1,0) <0){
        printf("error\n");
        exit(1);
    }

    // 수신된 메시지 출력
    printf("Received message: %s\n", mbuf.mtext);

    msgctl(msgid, IPC_RMID, 0);



}
