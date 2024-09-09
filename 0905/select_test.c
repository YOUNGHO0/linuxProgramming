#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>

int main() {
    fd_set readFd;  // 읽기 가능한 파일 디스크립터 집합
    struct timeval timeout;  // select의 타임아웃을 위한 구조체
    int ret;
    char buf[256];  // 입력을 저장할 버퍼

    while (1) {
        // fd_set 초기화
        FD_ZERO(&readFd);
        FD_SET(STDIN_FILENO, &readFd);  // 표준 입력을 감시하도록 설정

        // 타임아웃 설정 (0초 1000마이크로초)
        timeout.tv_sec = 0;
        timeout.tv_usec = 1000;

        // select 함수 호출: 표준 입력에 데이터가 있으면 읽음
        ret = select(STDIN_FILENO + 1, &readFd, NULL, NULL, &timeout);

        // select 함수가 0보다 큰 값을 반환할 때만 동작
        if (ret > 0 && FD_ISSET(STDIN_FILENO, &readFd)) {
            // 표준 입력에서 문자열 읽기
            fgets(buf, sizeof(buf), stdin);
            printf("입력된 문자열: %s", buf);  // 입력된 문자열 출력
        }
    }

    return 0;
}
