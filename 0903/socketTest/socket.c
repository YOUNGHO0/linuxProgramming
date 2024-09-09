#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>


int main(){

    int ret, sock_fd[2];
    int status;
    char buf[] = "hello world", line[BUFSIZ];
    pid_t pid;

    ret = socketpair(AF_LOCAL,SOCK_STREAM,0,sock_fd);
    if(ret == -1){
        perror("socketError");
        return -1;
    }

    printf("socket 1 : %d\n", sock_fd[0]);
    printf("socket 2 : %d\n", sock_fd[1]);

    if((pid = fork()) < 0){
        perror("fork");
    }else if(pid == 0){
        write(sock_fd[0],buf,strlen(buf)+1);
        printf("Data send %s \n", buf );
        printf("Data send %s \n", buf );
        int n = read(sock_fd[0],buf,sizeof(buf));
        buf[n] = '\0';
        printf("from Parent : %s\n", buf);

        close(sock_fd[0]);
    }
    else{

        read(sock_fd[1],line, BUFSIZ);
        printf("received data : %s\n", line);
        write(sock_fd[1],"HI",2);

        //wait(&status);
        close(sock_fd[1]);
    }

    return 0;
}