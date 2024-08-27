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
    // handLer설정 먜크로로 function pointer로써 defined 되어있다.
    sigact.sa_handler = handler;
    //특정 시그널이 처리되는 동안 마스킹 되어있는 시그널들을 블록한다.
    sigfillset(&sigact.sa_mask);
    // , read()나 write()와 같은 시스템 호출이 시그널에 의해 중단되었을 때
    // 이 플래그가 설정되면 시스템 호출이 자동으로 재시작됩니다.
    sigact.sa_flags = SA_RESTART;
    // inturrupt가 들어오면 sigact가 수행됩니다. 이전의 act는 oldact에 저장됩니다.
    sigaction(SIGINT,&sigact, &oldsigact);

//    signal(SIGINT,handler);

    signal(SIGKILL, handler);
    signal(SIGINT, handler);
    for(;;){
        printf("signal test\n") ;
        sleep(1);
    }


}
