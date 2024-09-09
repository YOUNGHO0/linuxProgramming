#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <stdlib.h>

#define TCP_PORT 5100            /* ������ ��Ʈ ��ȣ */
#define MAX_CLIENTS 10           /* �ִ� Ŭ���̾�Ʈ �� ���� */
#define MAX_EVENTS 20            /* epoll �̺�Ʈ�� �ִ� �� */

int main(int argc, char **argv) {
    int ssock;                   /* ���� ���� ��ũ���� */
    socklen_t clen;
    struct sockaddr_in servaddr, cliaddr;  /* �ּ� ����ü ���� */
    char mesg[BUFSIZ];
    int epoll_fd, nfds;
    struct epoll_event ev, events[MAX_EVENTS];
    int client_socks[MAX_CLIENTS]; /* ����� Ŭ���̾�Ʈ ���� �迭 */
    int client_count = 0;          /* ���� ����� Ŭ���̾�Ʈ �� */

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

    /* epoll �ν��Ͻ� ���� */
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1()");
        return -1;
    }

    /* ���� ������ epoll�� ��� */
    ev.events = EPOLLIN;   // �Է� �̺�Ʈ ����
    ev.data.fd = ssock;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ssock, &ev) == -1) {
        perror("epoll_ctl: ssock");
        return -1;
    }

    while (1) {
        /* epoll ��� */
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            return -1;
        }

        for (int n = 0; n < nfds; n++) {
            if (events[n].data.fd == ssock) {  // ���ο� Ŭ���̾�Ʈ ����
                int csock = accept(ssock, (struct sockaddr *)&cliaddr, &clen);
                if (csock < 0) {
                    perror("accept()");
                    continue;
                }

                printf("Client connected: %d\n", csock);

                /* Ŭ���̾�Ʈ ������ epoll�� �߰� */
                ev.events = EPOLLIN | EPOLLET;  // �Է� �̺�Ʈ �� ���� Ʈ���� ��� ����
                ev.data.fd = csock;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, csock, &ev) == -1) {
                    perror("epoll_ctl: csock");
                    close(csock);
                    continue;
                }

                /* Ŭ���̾�Ʈ ���� �迭�� �߰� */
                if (client_count < MAX_CLIENTS) {
                    client_socks[client_count++] = csock;
                } else {
                    printf("Max clients reached, closing connection: %d\n", csock);
                    close(csock);
                }
            } else {  // Ŭ���̾�Ʈ ���Ͽ��� ������ ����
                int csock = events[n].data.fd;
                int len = recv(csock, mesg, sizeof(mesg) - 1, 0);

                if (len <= 0) {  // ���� ���� ó��
                    if (len == 0) {
                        printf("Client disconnected: %d\n", csock);
                    } else {
                        perror("recv()");
                    }

                    close(csock);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, csock, NULL);  // epoll���� ���� ����

                    // ������ ����� Ŭ���̾�Ʈ�� �迭���� ����
                    for (int i = 0; i < client_count; i++) {
                        if (client_socks[i] == csock) {
                            client_socks[i] = client_socks[--client_count];
                            break;
                        }
                    }
                } else {
                    mesg[len] = '\0';  // �޽��� ����
                    printf("Received message from client %d: %s\n", csock, mesg);

                    // ���� �޽����� ��� ����� Ŭ���̾�Ʈ���� ����
                    for (int i = 0; i < client_count; i++) {
                        int client_fd = client_socks[i];
                        if (client_fd != csock) {  // ���ο��Դ� ������ ����
                            if (send(client_fd, mesg, len, MSG_DONTWAIT) <= 0) {
                                perror("send()");
                            }
                        }
                    }
                }
            }
        }
    }

    close(ssock);  /* ���� ������ ���� */
    close(epoll_fd);  /* epoll ���� ��ũ���� ���� */
    return 0;
}
