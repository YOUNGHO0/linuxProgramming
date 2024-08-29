#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(){

    pid_t cpid;
    int status;
    cpid = fork();
    switch(cpid){
        case -1:
            perror("fork");
            exit(1);
            break;
        case 0:     // child process

            printf("pid : %d , ppif : %d\n", getpid(), getppid());
            sleep(2);
            exit(2);

        default:  // parent process
            printf("Cpid : %d   , pid : %d\n", cpid, getppid());
           // sleep(5);
            wait(&status);
            if(WEXITSTATUS(status)){
                printf("exit code : %d\n", WEXITSTATUS(status));
            }else if(WIFSIGNALED(status)){
                printf("killed by signal\n");
            }

            system("ps -l");

            break;
    }
}