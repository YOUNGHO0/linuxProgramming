#include <stdio.h>
#include "minishell.h"
#include <stdlib.h>
#include <unistd.h>
void run_command(char **arglist) {
    pid_t cpid;

    switch (cpid = fork()) {
        case -1:  // fork ����
            perror("fork");
            exit(1);

        case 0:  // �ڽ� ���μ���
            printf("%s || %s \n",arglist[0],arglist[1]);
            execvp(arglist[0], arglist);

            perror("exec");  // execv�� ������ ���
            exit(1);

        default:  // �θ� ���μ���
            waitpid(cpid, NULL, 0);  // �ڽ� ���μ����� ���� ������ ��ٸ�
            printf("parent process\n");
            break;
    }
}
