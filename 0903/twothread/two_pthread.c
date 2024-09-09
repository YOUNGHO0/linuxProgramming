#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define LOOP_MAX 2000

int commonCounter= 0;
pthread_mutex_t mutexid;

void * inc_thread(void *);

int main(void)
{
     pthread_t tid1;
     pthread_t tid2;
     //mutex ����
    pthread_mutex_init(&mutexid, NULL);


     if ((pthread_create( &tid1, NULL, inc_thread, "thread1")) ||
         (pthread_create( &tid2, NULL, inc_thread, "thread2"))) {
          perror("pthread_create");
          exit (errno);
     }



    pthread_join(tid1, (void **)NULL);
    pthread_join(tid2, (void **)NULL);
    pthread_mutex_destroy(&mutexid);
    return 0;
}

void * inc_thread(void *arg)
{
    int loopCount;
    int temp;
    char buffer[80];
    int i;



    for (loopCount = 0; loopCount < LOOP_MAX; loopCount++) {
         //mutex lock
        int res = pthread_mutex_lock(&mutexid);
        if(res !=0){
            perror("lock error");
        }

        sprintf (buffer, "<%s> Common counter :  from %d to ", (char*) arg, commonCounter);
        write(1, buffer, strlen(buffer));

        temp = commonCounter;
        for(i=0; i<900000; i++);        // for delay
        commonCounter = temp + 1;
        sprintf (buffer, "%d\n", commonCounter);
        write(1, buffer, strlen(buffer));
        for(i=0; i<500000; i++);        // for delay

        //unlock

        int lockResult=pthread_mutex_unlock(&mutexid);
        if(lockResult !=0){
            perror(&mutexid);
        }
    }




    //mutex �Ҹ�

}

