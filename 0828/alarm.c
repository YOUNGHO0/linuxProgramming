//
// Created by lee on 2024. 8. 28..
//
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


void alarm_handler(int signalNumber){
    printf("alarm called\n");
    alarm(5);
}

int main(){

    signal(SIGALRM, alarm_handler);
    alarm(5);
    for(;;);

}