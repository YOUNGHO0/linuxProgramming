#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_PORT 5100 		/* ������ ��Ʈ ��ȣ */

int main(int argc, char **argv)
{
    int ssock;
    socklen_t clen;
    int n;
    struct sockaddr_in servaddr, cliaddr;
    char mesg[BUFSIZ];
    fd_set readfd; 			/* select( ) �Լ��� ���� �ڷ��� */
    int maxfd, client_index, start_index;
    int client_fd[5] = {0}; 		/* Ŭ���̾�Ʈ�� ���� FD �迭 */

    /* ���� ���� ��ũ���� ����. */
    if((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr)); 	/* �ü���� ���� ��� */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);

    if(bind(ssock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind()");
        return -1;
    }

    if(listen(ssock, 8) < 0) { 		/* Ŭ���̾�Ʈ�� ���ϵ��� ���� ť ���� */
        perror("listen()");
        return -1;
    }

    FD_ZERO(&readfd); 			/* fd_set �ڷ����� ��� 0���� �ʱ�ȭ */
    maxfd = ssock; 			/* ���� �ִ� ���� ��ũ������ ��ȣ�� ���� ������ ��ũ���� */
    client_index = 0;

    do {
        FD_SET(ssock, &readfd); 	/* �б� ���� ������ ���� fd_set �ڷ��� ���� */

        /* Ŭ���̾�Ʈ�� ���� �ּҺ��� ������ �ּұ��� fd_set �ڷ����� ���� */
        for(start_index = 0; start_index < client_index; start_index++) {
            FD_SET(client_fd[start_index], &readfd);
            if(client_fd[start_index] > maxfd)
                maxfd = client_fd[start_index]; 	/* ���� ū ������ ��ȣ�� ���� */
        }
        maxfd = maxfd + 1;

        /* select( ) �Լ����� �бⰡ ������ �κи� ���� */
        select(maxfd, &readfd, NULL, NULL, NULL); 	/* �бⰡ �������� ������ ���ŷ */
        if(FD_ISSET(ssock, &readfd)) { 			/* �бⰡ ������ ������ ���� ������ ��� */
            clen = sizeof(struct sockaddr_in);		/* Ŭ���̾�Ʈ�� ��û �޾Ƶ��̱� */
            int csock = accept(ssock, (struct sockaddr*)&cliaddr, &clen);
            if(csock < 0) {
                perror("accept()");
                return -1;
            } else {
                /* ��Ʈ��ũ �ּҸ� ���ڿ��� ���� */
                inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);
                printf("Client is connected : %s\n", mesg);

                /* ���� ������ Ŭ���̾�Ʈ�� ���� ��ȣ�� fd_set�� �߰� */
                FD_SET(csock, &readfd);
                client_fd[client_index] = csock;
                client_index++;
                continue;
            }
            if (client_index == 5) break;
        }

        /* �б� �����ߴ� ������ Ŭ���̾�Ʈ���� ��� */
        for(start_index = 0; start_index < client_index; start_index++) {
            /* for ������ �̿��ؼ� Ŭ���̾�Ʈ���� ��� ���� */
            if(FD_ISSET(client_fd[start_index], &readfd)) {
                memset(mesg, 0, sizeof(mesg));

                /* �ش� Ŭ���̾�Ʈ���� �޽����� �а� �ٽ� ����(Echo) */
                if((n = read(client_fd[start_index], mesg, sizeof(mesg))) > 0) {
                    printf("Received data : %s", mesg);
                    write(client_fd[start_index], mesg, n);
                    close(client_fd[start_index]); 	/* Ŭ���̾�Ʈ ������ �ݴ´�. */

                    /* Ŭ���̾�Ʈ ������ �����. */
                    FD_CLR(client_fd[start_index], &readfd);
                    client_index--;
                }
            }
        }
    } while(strncmp(mesg, "q", 1));

    close(ssock); 					/* ���� ������ �ݴ´�. */

    return 0;
}