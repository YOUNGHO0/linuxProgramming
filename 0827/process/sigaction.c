//
// Created by lee on 2024. 8. 27..
//

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int count =0;

void handler(int signo){
    printf("1");
    printf("2");
    printf("3");
    printf("4");
    printf("5");

    printf("int_haaaaaAndler\n") ;
    count = count +1;
}


int main(){
    struct sigaction sigact, oldsigact;
    // handLer���� ��ũ�η� function pointer�ν� defined �Ǿ��ִ�.
    sigact.sa_handler = handler;
    //Ư�� �ñ׳��� ó���Ǵ� ���� ����ŷ �Ǿ��ִ� �ñ׳ε��� ����Ѵ�.
    sigfillset(&sigact.sa_mask);
    // , read()�� write()�� ���� �ý��� ȣ���� �ñ׳ο� ���� �ߴܵǾ��� ��
    // �� �÷��װ� �����Ǹ� �ý��� ȣ���� �ڵ����� ����۵˴ϴ�.
    sigact.sa_flags = SA_RESTART;
    // inturrupt�� ������ sigact�� ����˴ϴ�. ������ act�� oldact�� ����˴ϴ�.
    sigaction(SIGINT,&sigact, &oldsigact);

//    signal(SIGINT,handler);

    signal(SIGKILL, handler);
    signal(SIGINT, handler);
    for(;;){
        printf("signal test\n") ;
        sleep(1);
    }


}
