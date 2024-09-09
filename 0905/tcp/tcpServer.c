#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define TCP_PORT 5100               /* 서버의 포트 번호 */
#define MAX_CLIENTS 10              /* 최대 클라이언트 수 정의 */

int main(int argc, char **argv)
{
    int ssock;                      /* 서버 소켓 디스크립터 */
    socklen_t clen;
    struct sockaddr_in servaddr, cliaddr;  /* 주소 구조체 정의 */
    char mesg[BUFSIZ];
    int client_socks[MAX_CLIENTS];  /* 클라이언트 소켓들을 저장하는 배열 */
    int client_count = 0;           /* 현재 연결된 클라이언트 수 */

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

    while (1) {
        /* 새로운 클라이언트 연결을 수락 */
        int csock = accept(ssock, (struct sockaddr *)&cliaddr, &clen);
        if (csock < 0) {
            perror("accept()");
            continue;
        }

        /* 클라이언트 소켓 추가 */
        if (client_count < MAX_CLIENTS) {
            client_socks[client_count++] = csock;
            printf("Client connected: %d\n", csock);
        } else {
            printf("Max clients reached, connection refused.\n");
            close(csock);
            continue;
        }

        /* fork를 사용하여 클라이언트 처리 */
        if (fork() == 0) { // child process
            while (1) {
                fd_set readFd;
                FD_ZERO(&readFd);                  // fd_set 초기화
                FD_SET(csock, &readFd);            // 소켓 입력 감시
                FD_SET(STDIN_FILENO, &readFd);     // 표준 입력 감시

                // 감시할 최대 파일 디스크립터 설정
                int max_fd = (STDIN_FILENO > csock) ? STDIN_FILENO : csock;

                // select 호출: 키보드와 소켓 입력을 감시
                int ret = select(max_fd + 1, &readFd, NULL, NULL, NULL);
                if (ret < 0) {
                    perror("select()");
                    return -1;
                }

                // 키보드 입력 감지
                if (FD_ISSET(STDIN_FILENO, &readFd)) {
                    if (fgets(mesg, sizeof(mesg), stdin) != NULL) {
                        // 입력된 메시지를 모든 클라이언트에게 전송
                        for (int i = 0; i < client_count; i++) {
                            if (send(client_socks[i], mesg, strlen(mesg), MSG_DONTWAIT) <= 0) {
                                perror("send()");
                            }
                        }
                    }
                }

                // 소켓에서 데이터 수신 감지
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
                    mesg[n] = '\0'; // 받은 메시지 null 종료
                    printf("Received message from client %d: %s\n", csock, mesg);

                    // 받은 메시지를 모든 다른 클라이언트에게 전송
                    for (int i = 0; i < client_count; i++) {
                        if (client_socks[i] != csock) { // 본인에게는 보내지 않음
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

        /* 종료된 자식 프로세스 정리 */
        while (waitpid(-1, NULL, WNOHANG) > 0);

        // 연결이 종료된 클라이언트 정리
        for (int i = 0; i < client_count; i++) {
            if (client_socks[i] == 0) {
                // 소켓이 종료되었다면 제거
                client_socks[i] = client_socks[--client_count];
                i--;
            }
        }
    }

    close(ssock);  /* 서버 소켓을 닫음 */
    return 0;
}
