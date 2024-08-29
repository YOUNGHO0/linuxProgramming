#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(){

    int pfd[2];
    pipe(pfd); // pfd[0] read pipe fd pfd[1] write pipe fd
    // 012 는 이미 예약되어 있으니깐 3 4 받아옴
    // 항상[0]이 readd [1]이 write
    printf("%d, %d\n", pfd[0],pfd[1]);
    int n;
    char buf[] = "hello parent";
    char buf1[1024];
    switch(fork()){
        case -1:
            perror("fork");
            break;
        case 0:
//            dup2(pfd[1],1);
//            close(pfd[1]);
            close(pfd[0]);
             write(pfd[1], buf,strlen(buf));
//            execlp("ps", "ps", NULL);
            perror("execLp");
            exit(0);
            break;
        default:
            close(pfd[1]);
            n = read(pfd[0], buf1, sizeof(buf1));
            buf1[n] = '\0';
            printf("parent : %s\n",buf1);


    }



}