#include <stdio.h>
#include "minishell.h"
#include <stdlib.h>
#include <unistd.h>
void run_command(char **arglist) {
    pid_t cpid;

    switch (cpid = fork()) {
        case -1:  // fork 실패
            perror("fork");
            exit(1);

        case 0:  // 자식 프로세스
            printf("%s || %s \n",arglist[0],arglist[1]);
            execvp(arglist[0], arglist);

            perror("exec");  // execv가 실패한 경우
            exit(1);

        default:  // 부모 프로세스
            waitpid(cpid, NULL, 0);  // 자식 프로세스가 끝날 때까지 기다림
            printf("parent process\n");
            break;
    }
}
