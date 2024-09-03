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
    // handLer설정 먜크로로 function pointer로써 defined 되어있다.
    sigact.sa_handler = handler;
    //특정 시그널이 처리되는 동안 마스킹 되어있는 시그널들을 블록한다.
    sigfillset(&sigact.sa_mask);
    // , read()나 write()와 같은 시스템 호출이 시그널에 의해 중단되었을 때
    // 이 플래그가 설정되면 시스템 호출이 자동으로 재시작됩니다.
    sigact.sa_flags = SA_RESTART;
    // inturrupt가 들어오면 sigact가 수행됩니다. 이전의 act는 oldact에 저장됩니다.
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