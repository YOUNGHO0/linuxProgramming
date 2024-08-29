#include <stdbool.h>
#include <unistd.h>


int is_builtin(char ** arg){

    int num = execvp(arg[0],arg);
    return 1;
    return 0;
}

void cd_func(char ** arg){
    if(chdir(arg[1]) == 1){

    }
}