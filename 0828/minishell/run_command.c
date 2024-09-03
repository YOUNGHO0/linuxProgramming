#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void run_command(char** arglist)
{
    pid_t cpid;

    switch(cpid=fork()) {
        case -1:
        break;

        case 0:
        execvp(arglist[0], arglist);
        perror("execvp");
        exit(1);
        break;

        default:
        waitpid(cpid, NULL, 0);
        break;
    }
 
}