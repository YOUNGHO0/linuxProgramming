#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

int main(){

    int status;
    if(fork() == 0){

        printf("main start\n");
        //exec 는 코드를 교체하는 함수라서 실패하면 아래가 실행된다.
//    execl("/bin/ls1","ls","-l",NULL);
        // 경로 지정 필요 없음
//    execlp("ls","ls","-l",NULL);
        char * arglist[] = {"sleep", "sleep", "5", NULL};
        execvp(arglist[0],&arglist[1]);
        printf("main end\n");
        exit(0);
    }
    else{
        wait(&status);
        if(WEXITSTATUS(status)){
            printf("exit code : %d\n", WEXITSTATUS(status));
        }else if(WIFSIGNALED(status)){
            printf("killed by signal\n");
        }
//        printf("%d", status);
    }


}
