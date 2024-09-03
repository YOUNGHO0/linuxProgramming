#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sem.h>
int main()
{
	int shmid, i, j;
	char *shmaddr;
    struct sembuf buf;

	if((shmid=shmget(0x123400, 30, 0660|IPC_CREAT|IPC_EXCL))==-1) {
		if((shmid=shmget(0x123400, 30, 0660))==-1){
			perror("shmget");
			exit(1);
		}
	}
 	if((shmaddr= shmat(shmid, (char *)0, 0))== NULL) {
		perror("shmat");
		exit(1);
	}
 	while(1) {

         //세마포어 P연산

 		if(!strcmp(shmaddr,"end"))
 			break;
 		if(!strcmp(shmaddr,""))
 			continue;
 		printf("recv : %s\n", shmaddr);
		for(j=0; j<999999999; j++);
        //세마포어 V 연산
     }


	if(shmdt(shmaddr) == -1 ) {
		perror("shmdt");
		exit(1);
	}
	if(shmctl(shmid, IPC_RMID, (struct shmid_ds *)0) == -1 ) {
		perror("shmctl");
		exit(1);
	}

    // 세마포어 삭제

	return 0;
}

