//
// Created by lee on 2024. 8. 27..
//

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int count =0;

void handler(int signo){

    printf("int_handler\n") ;
    count = count +1;
}


int main(){
    // �ñ׳��� �����ϰ� ó���� �ڵ鷯�� ����Ѵ�.
    signal(SIGINT,SIG_IGN);
    signal(SIGINT, handler);
    for(;;){
        printf("signal test\n") ;
        sleep(1);
    }


}
