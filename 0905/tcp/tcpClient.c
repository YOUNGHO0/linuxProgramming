#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define TCP_PORT 5100


int main(int argc, char **argv)
{
    int ssock;
    struct sockaddr_in servaddr;
    char mesg[BUFSIZ];

    if(argc < 2) {
        printf("Usage : %s IP_ADRESS\n", argv[0]);
        return -1;
    }

    /* ������ ���� */
    if((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    /* ������ ������ �ּ� ���� */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    /* ���ڿ��� ��Ʈ��ũ �ּҷ� ���� */
    inet_pton(AF_INET, argv[1], &(servaddr.sin_addr.s_addr));
    servaddr.sin_port = htons(TCP_PORT);

    /* ������ �ּҷ� ���� */
    if(connect(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect()");
        return -1;
    }
    while(1) {
        fd_set readFd;
        FD_ZERO(&readFd);                  // fd_set �ʱ�ȭ
        FD_SET(STDIN_FILENO, &readFd);     // ǥ�� �Է� ����
        FD_SET(ssock, &readFd);            // ���� �Է� ����

        // ������ �ִ� ���� ��ũ���� ����
        int max_fd = (STDIN_FILENO > ssock) ? STDIN_FILENO : ssock;

        // select ȣ��: Ű����� ���� �Է��� ����
        int ret = select(max_fd + 1, &readFd, NULL, NULL, NULL);
        if (ret < 0) {
            perror("select()");
            return -1;
        }

        // Ű���� �Է� ����
        if (FD_ISSET(STDIN_FILENO, &readFd)) {
            if (fgets(mesg, sizeof(mesg), stdin) != NULL) {
                // �������� �����͸� �񵿱�� ����
                if (send(ssock, mesg, strlen(mesg), MSG_DONTWAIT) <= 0) {
                    perror("send()");
                    return -1;
                }
            }


        }
        // ���Ͽ��� ������ ���� ����
        else if (FD_ISSET(ssock, &readFd)) {
            int n = recv(ssock, mesg, sizeof(mesg) - 1, 0);
            if (n <= 0) {
                if (n == 0) {
                    printf("������ ������ �����߽��ϴ�.\n");
                } else {
                    perror("recv()");
                }
                break;
            }
            mesg[n] = '\0'; // ���� �޽��� null ����
            printf("Received data : %s", mesg); 		/* �޾ƿ� ���ڿ��� ȭ�鿡 ��� */
        }


    }
    close(ssock); 					/* ������ �ݴ´�. */

    return 0;
}