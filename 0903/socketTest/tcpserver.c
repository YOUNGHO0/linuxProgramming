#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <stdlib.h>

#define TCP_PORT 5100 				/* ������ ��Ʈ ��ȣ */

int main(int argc, char **argv)
{
    int ssock; 					/* ���� ��ũ��Ʈ ���� */
    socklen_t clen;
    int n;
    struct sockaddr_in servaddr, cliaddr; 	/* �ּ� ����ü ���� */
    char mesg[BUFSIZ];

    /* ���� ���� ���� */
    if((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    /* �ּ� ����ü�� �ּ� ���� */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(TCP_PORT); 	/* ����� ��Ʈ ���� */

    /* bind �Լ��� ����Ͽ� ���� ������ �ּ� ���� */
    if(bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind()");
        return -1;
    }

    /* ���ÿ� �����ϴ� Ŭ���̾�Ʈ�� ó���� ���� ��� ť�� ���� */
    if(listen(ssock, 8) < 0) {
        perror("listen()");
        return -1;
    }

    clen = sizeof(cliaddr);
    do {
        /* Ŭ���̾�Ʈ�� �����ϸ� ������ ����ϰ� Ŭ���̾�Ʈ ���� ���� */
        int n, csock = accept(ssock, (struct sockaddr *)&cliaddr, &clen);
        pid_t  childPid;
        int status;
        printf("Client is connected : %s\n", mesg);
        switch (childPid = fork()) {
            case 0:
                inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);
                printf("Client is connected : %s\n", mesg);
                if((n = read(csock, mesg, BUFSIZ)) <= 0)
                    perror("read()");
                printf("Received data : %s", mesg);

                /* Ŭ���̾�Ʈ�� buf�� �ִ� ���ڿ� ���� */
                if(write(csock, mesg, n) <= 0)
                    perror("write()");
                close(csock); 			/* Ŭ���̾�Ʈ ������ ���� */
                exit(0);
                break;
            default:
                while(waitpid(0,&status,WNOHANG) >0);

                break;
        }
        /* ��Ʈ��ũ �ּҸ� ���ڿ��� ���� */

    } while(strncmp(mesg, "q", 1));

    close(ssock); 			/* ���� ������ ���� */

    return 0;
}