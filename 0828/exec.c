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
        //exec �� �ڵ带 ��ü�ϴ� �Լ��� �����ϸ� �Ʒ��� ����ȴ�.
//    execl("/bin/ls1","ls","-l",NULL);
        // ��� ���� �ʿ� ����
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
