#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <stdlib.h>

#define TCP_PORT 5100            /* 서버의 포트 번호 */
#define MAX_CLIENTS 10           /* 최대 클라이언트 수 정의 */
#define MAX_EVENTS 20            /* epoll 이벤트의 최대 수 */

int main(int argc, char **argv) {
    int ssock;                   /* 서버 소켓 디스크립터 */
    socklen_t clen;
    struct sockaddr_in servaddr, cliaddr;  /* 주소 구조체 정의 */
    char mesg[BUFSIZ];
    int epoll_fd, nfds;
    struct epoll_event ev, events[MAX_EVENTS];
    int client_socks[MAX_CLIENTS]; /* 연결된 클라이언트 소켓 배열 */
    int client_count = 0;          /* 현재 연결된 클라이언트 수 */

    /* 서버 소켓 생성 */
    if ((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    /* 주소 구조체에 주소 지정 */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(TCP_PORT);  /* 사용할 포트 지정 */

    /* bind 함수를 사용하여 서버 소켓의 주소 설정 */
    if (bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind()");
        return -1;
    }

    /* 클라이언트의 처리를 위한 대기 큐를 설정 */
    if (listen(ssock, 8) < 0) {
        perror("listen()");
        return -1;
    }

    clen = sizeof(cliaddr);
    memset(client_socks, 0, sizeof(client_socks));  /* 클라이언트 소켓 배열 초기화 */

    /* epoll 인스턴스 생성 */
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1()");
        return -1;
    }

    /* 서버 소켓을 epoll에 등록 */
    ev.events = EPOLLIN;   // 입력 이벤트 설정
    ev.data.fd = ssock;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ssock, &ev) == -1) {
        perror("epoll_ctl: ssock");
        return -1;
    }

    while (1) {
        /* epoll 대기 */
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            return -1;
        }

        for (int n = 0; n < nfds; n++) {
            if (events[n].data.fd == ssock) {  // 새로운 클라이언트 연결
                int csock = accept(ssock, (struct sockaddr *)&cliaddr, &clen);
                if (csock < 0) {
                    perror("accept()");
                    continue;
                }

                printf("Client connected: %d\n", csock);

                /* 클라이언트 소켓을 epoll에 추가 */
                ev.events = EPOLLIN | EPOLLET;  // 입력 이벤트 및 엣지 트리거 모드 설정
                ev.data.fd = csock;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, csock, &ev) == -1) {
                    perror("epoll_ctl: csock");
                    close(csock);
                    continue;
                }

                /* 클라이언트 소켓 배열에 추가 */
                if (client_count < MAX_CLIENTS) {
                    client_socks[client_count++] = csock;
                } else {
                    printf("Max clients reached, closing connection: %d\n", csock);
                    close(csock);
                }
            } else {  // 클라이언트 소켓에서 데이터 수신
                int csock = events[n].data.fd;
                int len = recv(csock, mesg, sizeof(mesg) - 1, 0);

                if (len <= 0) {  // 연결 종료 처리
                    if (len == 0) {
                        printf("Client disconnected: %d\n", csock);
                    } else {
                        perror("recv()");
                    }

                    close(csock);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, csock, NULL);  // epoll에서 소켓 제거

                    // 연결이 종료된 클라이언트를 배열에서 제거
                    for (int i = 0; i < client_count; i++) {
                        if (client_socks[i] == csock) {
                            client_socks[i] = client_socks[--client_count];
                            break;
                        }
                    }
                } else {
                    mesg[len] = '\0';  // 메시지 종료
                    printf("Received message from client %d: %s\n", csock, mesg);

                    // 받은 메시지를 모든 연결된 클라이언트에게 전송
                    for (int i = 0; i < client_count; i++) {
                        int client_fd = client_socks[i];
                        if (client_fd != csock) {  // 본인에게는 보내지 않음
                            if (send(client_fd, mesg, len, MSG_DONTWAIT) <= 0) {
                                perror("send()");
                            }
                        }
                    }
                }
            }
        }
    }

    close(ssock);  /* 서버 소켓을 닫음 */
    close(epoll_fd);  /* epoll 파일 디스크립터 닫음 */
    return 0;
}
