#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "minishell.h"
#include <signal.h>
#define MAXLINE 255


void handler(int x){
    printf("thisid : %d\n", x);
    int status;
    pid_t  pid;

    pid = waitpid(-1, &status, WNOHANG);

    if(pid !=0){
        if (WIFEXITED(status)) {
            printf("Child process %d exited with status %d\n", pid, WEXITSTATUS(status));


        } else if (WIFSIGNALED(status)) {
            printf("Child process %d was terminated by signal %d\n", pid, WTERMSIG(status));
        }

    }
}

int main(void) {
    char line[MAXLINE];
    char **arglist;

    struct sigaction sigact, oldsigact;
    // handLer���� ��ũ�η� function pointer�ν� defined �Ǿ��ִ�.
    sigact.sa_handler = handler;
    //Ư�� �ñ׳��� ó���Ǵ� ���� ����ŷ �Ǿ��ִ� �ñ׳ε��� ����Ѵ�.
    sigfillset(&sigact.sa_mask);
    // , read()�� write()�� ���� �ý��� ȣ���� �ñ׳ο� ���� �ߴܵǾ��� ��
    // �� �÷��װ� �����Ǹ� �ý��� ȣ���� �ڵ����� ����۵˴ϴ�.
    sigact.sa_flags = SA_RESTART;
    // inturrupt�� ������ sigact�� ����˴ϴ�. ������ act�� oldact�� ����˴ϴ�.
    sigaction(SIGCHLD,&sigact, &oldsigact);
    fputs("minishell> ",stdout);

    while (fgets(line,MAXLINE,stdin)) {
        if(!strncmp(line,"exit",4))
            exit(0);

        arglist=command_parse(line);

        // for(int i=0; arglist[i]!=NULL; i++)
        //     printf("%s\n", arglist[i]);
        if(!is_builtin(arglist))
            run_command(arglist);
        command_freelist(arglist);
        fputs("minishell> ",stdout);
    }
    return 0;
}