#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define TCP_PORT 5100               /* ������ ��Ʈ ��ȣ */
#define MAX_CLIENTS 10              /* �ִ� Ŭ���̾�Ʈ �� ���� */

int main(int argc, char **argv)
{
    int ssock;                      /* ���� ���� ��ũ���� */
    socklen_t clen;
    struct sockaddr_in servaddr, cliaddr;  /* �ּ� ����ü ���� */
    char mesg[BUFSIZ];
    int client_socks[MAX_CLIENTS];  /* Ŭ���̾�Ʈ ���ϵ��� �����ϴ� �迭 */
    int client_count = 0;           /* ���� ����� Ŭ���̾�Ʈ �� */

    /* ���� ���� ���� */
    if ((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    /* �ּ� ����ü�� �ּ� ���� */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(TCP_PORT);  /* ����� ��Ʈ ���� */

    /* bind �Լ��� ����Ͽ� ���� ������ �ּ� ���� */
    if (bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind()");
        return -1;
    }

    /* Ŭ���̾�Ʈ�� ó���� ���� ��� ť�� ���� */
    if (listen(ssock, 8) < 0) {
        perror("listen()");
        return -1;
    }

    clen = sizeof(cliaddr);
    memset(client_socks, 0, sizeof(client_socks));  /* Ŭ���̾�Ʈ ���� �迭 �ʱ�ȭ */

    while (1) {
        /* ���ο� Ŭ���̾�Ʈ ������ ���� */
        int csock = accept(ssock, (struct sockaddr *)&cliaddr, &clen);
        if (csock < 0) {
            perror("accept()");
            continue;
        }

        /* Ŭ���̾�Ʈ ���� �߰� */
        if (client_count < MAX_CLIENTS) {
            client_socks[client_count++] = csock;
            printf("Client connected: %d\n", csock);
        } else {
            printf("Max clients reached, connection refused.\n");
            close(csock);
            continue;
        }

        /* fork�� ����Ͽ� Ŭ���̾�Ʈ ó�� */
        if (fork() == 0) { // child process
            while (1) {
                fd_set readFd;
                FD_ZERO(&readFd);                  // fd_set �ʱ�ȭ
                FD_SET(csock, &readFd);            // ���� �Է� ����
                FD_SET(STDIN_FILENO, &readFd);     // ǥ�� �Է� ����

                // ������ �ִ� ���� ��ũ���� ����
                int max_fd = (STDIN_FILENO > csock) ? STDIN_FILENO : csock;

                // select ȣ��: Ű����� ���� �Է��� ����
                int ret = select(max_fd + 1, &readFd, NULL, NULL, NULL);
                if (ret < 0) {
                    perror("select()");
                    return -1;
                }

                // Ű���� �Է� ����
                if (FD_ISSET(STDIN_FILENO, &readFd)) {
                    if (fgets(mesg, sizeof(mesg), stdin) != NULL) {
                        // �Էµ� �޽����� ��� Ŭ���̾�Ʈ���� ����
                        for (int i = 0; i < client_count; i++) {
                            if (send(client_socks[i], mesg, strlen(mesg), MSG_DONTWAIT) <= 0) {
                                perror("send()");
                            }
                        }
                    }
                }

                // ���Ͽ��� ������ ���� ����
                if (FD_ISSET(csock, &readFd)) {
                    int n = recv(csock, mesg, sizeof(mesg) - 1, 0);
                    if (n <= 0) {
                        if (n == 0) {
                            printf("Client disconnected: %d\n", csock);
                        } else {
                            perror("recv()");
                        }
                        break;
                    }
                    mesg[n] = '\0'; // ���� �޽��� null ����
                    printf("Received message from client %d: %s\n", csock, mesg);

                    // ���� �޽����� ��� �ٸ� Ŭ���̾�Ʈ���� ����
                    for (int i = 0; i < client_count; i++) {
                        if (client_socks[i] != csock) { // ���ο��Դ� ������ ����
                            if (send(client_socks[i], mesg, n, MSG_DONTWAIT) <= 0) {
                                perror("send()");
                            }
                        }
                    }
                }
            }
            close(csock);
            exit(0);
        }

        /* ����� �ڽ� ���μ��� ���� */
        while (waitpid(-1, NULL, WNOHANG) > 0);

        // ������ ����� Ŭ���̾�Ʈ ����
        for (int i = 0; i < client_count; i++) {
            if (client_socks[i] == 0) {
                // ������ ����Ǿ��ٸ� ����
                client_socks[i] = client_socks[--client_count];
                i--;
            }
        }
    }

    close(ssock);  /* ���� ������ ���� */
    return 0;
}
