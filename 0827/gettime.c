//
// Created by lee on 2024. 8. 27..
//

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>


int main(){

    struct timeval tv;
    struct timespec timespec;
    gettimeofday(&tv, NULL);
    printf("tv_sec : %ld, tv_usec : %d\n", tv.tv_sec, tv.tv_usec);
    clock_gettime(CLOCK_REALTIME, &timespec);
    printf("time_sec : %ld, time_usec : %d\n", timespec.tv_sec, timespec.tv_nsec);



}
