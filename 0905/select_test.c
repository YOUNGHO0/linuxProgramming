#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>

int main() {
    fd_set readFd;  // �б� ������ ���� ��ũ���� ����
    struct timeval timeout;  // select�� Ÿ�Ӿƿ��� ���� ����ü
    int ret;
    char buf[256];  // �Է��� ������ ����

    while (1) {
        // fd_set �ʱ�ȭ
        FD_ZERO(&readFd);
        FD_SET(STDIN_FILENO, &readFd);  // ǥ�� �Է��� �����ϵ��� ����

        // Ÿ�Ӿƿ� ���� (0�� 1000����ũ����)
        timeout.tv_sec = 0;
        timeout.tv_usec = 1000;

        // select �Լ� ȣ��: ǥ�� �Է¿� �����Ͱ� ������ ����
        ret = select(STDIN_FILENO + 1, &readFd, NULL, NULL, &timeout);

        // select �Լ��� 0���� ū ���� ��ȯ�� ���� ����
        if (ret > 0 && FD_ISSET(STDIN_FILENO, &readFd)) {
            // ǥ�� �Է¿��� ���ڿ� �б�
            fgets(buf, sizeof(buf), stdin);
            printf("�Էµ� ���ڿ�: %s", buf);  // �Էµ� ���ڿ� ���
        }
    }

    return 0;
}
