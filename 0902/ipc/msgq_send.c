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

    msgid = msgget(ftok(".", 100), IPC_CREAT|0644);
    if(msgid == -1){
        perror("msgget");
        exit(1);
    }
    mbuf.mtype = 1;
    strcpy(mbuf.mtext, "sender");
    msgsnd(msgid, &mbuf,sizeof(mbuf.mtext),IPC_NOWAIT);
    sleep(5);
    //    msgctl(msgid, IPC_RMID, 0);


}
