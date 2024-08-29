#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main()
{
    pid_t cpid;
    int status;
    cpid=fork();
    switch(cpid) {
        case -1:
            perror("fork");
            exit(1);
            break;
        case 0:     // child process
            printf("pid : %d, ppid : %d\n",
                   getpid(), getppid());
            sleep(8);
            exit(2);
            break;
        default:    // parent process
            printf("cpid : %d, pid : %d\n",
                   cpid, getpid());
            while(waitpid(cpid, &status, 0)!=cpid) {
                printf("parent process\n");
                sleep(1);
            }
            if(WIFEXITED(status)) {
                printf("exit code : %d\n",
                       WEXITSTATUS(status));
            } else if(WIFSIGNALED(status)) {
                printf("killed by signal\n");
            }
            system("ps -l");
            break;
    }
}