#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>

int main(){

    struct rlimit rlim;
    getrlimit(RLIMIT_NPROC, &rlim);
    printf("max user processx : %lu  / %lu \n", rlim.rlim_cur, rlim
    .rlim_max);


    getrlimit(RLIMIT_NOFILE, &rlim);
    printf("file size: %lu / %lu \n", rlim.rlim_cur, rlim.rlim_max);

    getrlimit(RLIMIT_RSS, &rlim);
    printf("max memory size : %1u / %1uln", rlim.rlim_cur, rlim.rlim_max);
    getrlimit(RLIMIT_CPU, &rlim) ;
    if(rlim.rlim_cur == RLIM_INFINITY){
        printf("cpu time : ULIMIT\n");
    }
    return 0;
}