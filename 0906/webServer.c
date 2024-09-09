#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* ������ ó���� ���� �Լ� */
static void *clnt_connection(void *arg);
int sendData(FILE* fp, char *ct, char *filename);
void sendOk(FILE* fp);
void sendError(FILE* fp);

int main(int argc, char **argv)
{
    int ssock;
    pthread_t thread;
    struct sockaddr_in servaddr, cliaddr;
    unsigned int len;

    /* ���α׷��� ������ �� ������ ���� ��Ʈ ��ȣ�� �Է¹޴´�. */
    if(argc!=2) {
        printf("usage: %s <port>\n", argv[0]);
        return -1;
    }

    /* ������ ���� ������ �����Ѵ�. */
    ssock = socket(AF_INET, SOCK_STREAM, 0);
    if(ssock == -1) {
        perror("socket()");
        return -1;
    }

    /* �Է¹޴� ��Ʈ ��ȣ�� �̿��ؼ� ���񽺸� �ü���� ����Ѵ�. */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = (argc != 2)?htons(8000):htons(atoi(argv[1]));
    if(bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr))==-1) {
        perror("bind()");
        return -1;
    }

    /* �ִ� 10���� Ŭ���̾�Ʈ�� ���� ������ ó���� �� �ֵ��� ť�� �����Ѵ�. */
    if(listen(ssock, 10) == -1) {
        perror("listen()");
        return -1;
    }

    while(1) {
        char mesg[BUFSIZ];
        int csock;

        /* Ŭ���̾�Ʈ�� ��û�� ��ٸ���. */
        len = sizeof(cliaddr);
        csock = accept(ssock, (struct sockaddr*)&cliaddr, &len);

        /* ��Ʈ��ũ �ּҸ� ���ڿ��� ���� */
        inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);
        printf("Client IP : %s:%d\n", mesg, ntohs(cliaddr.sin_port));

        /* Ŭ���̾�Ʈ�� ��û�� ������ �����带 �����ϰ� Ŭ���̾�Ʈ�� ��û�� ó���Ѵ�. */
        pthread_create(&thread, NULL, clnt_connection, &csock);
        pthread_join(thread, NULL);
    }
    return 0;
}

void *clnt_connection(void *arg)
{
    /* �����带 ���ؼ� �Ѿ�� arg�� int ���� ���� ��ũ���ͷ� ��ȯ�Ѵ�. */
    int csock = *((int*)arg);
    FILE *clnt_read, *clnt_write;
    char reg_line[BUFSIZ], reg_buf[BUFSIZ];
    char method[BUFSIZ], type[BUFSIZ];
    char filename[BUFSIZ], *ret;

    /* ���� ��ũ���͸� FILE ��Ʈ������ ��ȯ�Ѵ�. */
    clnt_read = fdopen(csock, "r");
    clnt_write = fdopen(dup(csock), "w");

    /* �� ���� ���ڿ��� �о reg_line ������ �����Ѵ�. */
    fgets(reg_line, BUFSIZ, clnt_read);

    /* reg_line ������ ���ڿ��� ȭ�鿡 ����Ѵ�. */
    fputs(reg_line, stdout);

    /* ' ' ���ڷ� reg_line�� �����ؼ� ��û ������ ����(�޼ҵ�)�� �и��Ѵ�. */
    ret = strtok(reg_line, "/ ");
    strcpy(method, (ret != NULL)?ret:"");
    if(strcmp(method, "POST") == 0) { 		/* POST �޼ҵ��� ��츦 ó���Ѵ�. */
        sendOk(clnt_write); 			/* �ܼ��� OK �޽����� Ŭ���̾�Ʈ�� ������. */
        goto END;
    } else if(strcmp(method, "GET") != 0) {	/* GET �޼ҵ尡 �ƴ� ��츦 ó���Ѵ�. */
        sendError(clnt_write); 			/* ���� �޽����� Ŭ���̾�Ʈ�� ������. */
        goto END;
    }

    ret = strtok(NULL, " ");
    printf("ret : %s\n",ret);
    ret = "/index.html";
    /* ��û ���ο��� ���(path)�� �����´�. */
    strcpy(filename, (ret != NULL)?ret:"");
    if(filename[0] == '/') { 			/* ��ΰ� '/'�� ���۵� ��� /�� �����Ѵ�. */
        for(int i = 0, j = 0; i < BUFSIZ; i++, j++) {
            if(filename[0] == '/') j++;
            filename[i] = filename[j];
            if(filename[j] == '\0') break;
        }
    }

    /* ��û�� �����Ͽ� ��� */
    /* ������ ��ġ�� ã�� �� ����ϴ� �Լ� strchr */
    char *params = strchr(filename, '?');  /* �Ķ���� ������ ã�� */
    char *param_key;                       /* �Ķ���Ͱ� ���� ���� �κ��� �Ҵ��ϴ� �ʱ� ���� */

    if (params != NULL) {
        *params = '\0';                    /* ���ϸ�� �Ķ���� �и� (���� ������ ù �� GET ��½� �̾ �ٿ��� ����� ��.)*/
        params++;                          /* �Ķ���� �κ��� ����Ŵ */

        /* ù ��° ��: GET /index.html */
        printf("GET %s \n", filename);

        /* �Ķ���Ͱ� ���� ���� ��� ��� ��� */
        /* ���ڿ� ����� �����ϱ� ���ؼ� ����ϴ� �Լ� : strtok */
        while ((param_key = strtok(params, "&=")) != NULL) {
            printf("%s\n", param_key);
            params = NULL;                  /* ���� strtok() ȣ�� �ÿ��� NULL�� ���� */
        }

    } else {
        /* �Ķ���Ͱ� ���� ��� ���ϸ� ��� */
        printf("GET /%s\n", filename);
    }

    printf("HTTP/1.1\n");
    printf("filename: %s\n", filename);


//    /* �޽��� ����� �о ȭ�鿡 ����ϰ� �������� �����Ѵ�. */
//    do {
//        fgets(reg_line, BUFSIZ, clnt_read);
//        fputs(reg_line, stdout);
//        strcpy(reg_buf, reg_line);
//        char* buf = strchr(reg_buf, ':');
//    } while(strncmp(reg_line, "\r\n", 2)); 	/* ��û ����� ��\r\n������ ������. */

    /* ������ �̸��� �̿��ؼ� Ŭ���̾�Ʈ�� ������ ������ ������. */
    sendData(clnt_write, type, filename);

    END:
    fclose(clnt_read); 				/* ������ ��Ʈ���� �ݴ´�. */
    fclose(clnt_write);
    pthread_exit(0); 				/* �����带 �����Ų��. */

    return (void*)NULL;
}

int sendData(FILE* fp, char *ct, char *filename)
{
    /* Ŭ���̾�Ʈ�� ���� ������ ���� ���� �޽��� */
    char protocol[ ] = "HTTP/1.1 200 OK\r\n";
    char server[ ] = "Server:Netscape-Enterprise/6.0\r\n";
    char cnt_type[ ] = "Content-Type:text/html\r\n";
    char end[ ] = "\r\n"; 			/* ���� ����� ���� �׻� \r\n */
    char buf[BUFSIZ];
    int fd, len;

    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_type, fp);
    fputs(end, fp);

    fd = open(filename, O_RDWR); 		/* ������ ����. */
    do {
        len = read(fd, buf, BUFSIZ); 		/* ������ �о Ŭ���̾�Ʈ�� ������. */
        fputs(buf, fp);
    } while(len == BUFSIZ);

    close(fd); 					/* ������ �ݴ´�. */

    return 0;
}

void sendOk(FILE* fp)
{
    /* Ŭ���̾�Ʈ�� ���� ������ ���� HTTP ���� �޽��� */
    char protocol[ ] = "HTTP/1.1 200 OK\r\n";
    char server[ ] = "Server: Netscape-Enterprise/6.0\r\n\r\n";

    fputs(protocol, fp);
    fputs(server, fp);
    fflush(fp);
}

void sendError(FILE* fp)
{
    /* Ŭ���̾�Ʈ�� ���� ���п� ���� HTTP ���� �޽��� */
    char protocol[ ] = "HTTP/1.1 400 Bad Request\r\n";
    char server[ ] = "Server: Netscape-Enterprise/6.0\r\n";
    char cnt_len[ ] = "Content-Length:1024\r\n";
    char cnt_type[ ] = "Content-Type:text/html\r\n\r\n";

    /* ȭ�鿡 ǥ�õ� HTML�� ���� */
    char content1[ ] = "<html><head><title>BAD Connection</title></head>";
    char content2[ ] = "<body><font size=+5>Bad Request</font></body></html>";
    printf("send_error\n");

    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_len, fp);
    fputs(cnt_type, fp);
    fputs(content1, fp);
    fputs(content2, fp);
    fflush(fp);
}