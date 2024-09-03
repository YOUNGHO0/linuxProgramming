#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
int is_background(char ** arglist);
void is_pipe(char ** argslist);
void redirect_out(char ** arglist);

void run_command(char** arglist)
{
    pid_t cpid;
    int back_flag = 0;
    if(is_background(arglist)){
        back_flag = 1;
    }
    switch(cpid=fork()) {
        case -1:
        break;

    case 0:
        signal(SIGINT,SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        redirect_out(arglist);
        is_pipe(arglist);
        execvp(arglist[0], arglist);
        perror("execvp");
        exit(1);
        break;

        default:
        if(back_flag == 0)
            waitpid(cpid, NULL, 0);
        break;
    }
 
}

int is_background(char ** arglist){
    int i = 0;
    while (arglist[i] != NULL) {
        i++;
    }
    // arglist[i]가 NULL이므로, arglist[i-1]이 마지막 요소
    if (i > 0 && strcmp(arglist[i-1], "&") == 0) {
        arglist[i-1] = NULL;  // '&'를 제거하고 NULL로 설정
        return 1;
    }
    return 0;
}

void redirect_out(char ** argslist){
    int i;
    int fd;
    for(int i =0; argslist[i]!= NULL; i++){
        if(strcmp(argslist[i], ">") == 0){
           fd = open(argslist[i+1] , O_CREAT | O_WRONLY | O_TRUNC, 0644);
           if(fd == -1){
               perror("Open");
               return;
           }
            dup2(fd,1);
            close(fd);
            argslist[i] = NULL;
        }

    }
}



void is_pipe(char** argslist){

    int i;
    int fd;
    int pfd[2];
    pid_t child;
    pipe(pfd);
    for(int i =0; argslist[i]!= NULL; i++){
        if(strcmp(argslist[i], "|") == 0){
            printf("hello\n");
            argslist[i] = NULL;
            switch ( child =fork()) {
                case -1:
                    break;
                case 0:
                    close(pfd[0]);
                    dup2(pfd[1], 1);
                    close(pfd[1]);
                    execvp(argslist[0],argslist);
                    // ls -l |
                    break;
                default:
                    close(pfd[1]);
                    dup2(pfd[0], 0);
                    close(pfd[0]);
                    execvp(argslist[i+1], &argslist[i+1]);
                    break;


            }
        }

    }


}






















