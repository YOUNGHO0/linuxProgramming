#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "minishell.h"

#define MAXLINE 255

int main(void) {
    char line[MAXLINE];
    char **arglist;
    fputs("minishell> ",stdout);

    while (fgets(line,MAXLINE,stdin)) {
        if(!strncmp(line,"exit",4))
            exit(0);
//        fputs(line, stdout);
        arglist = command_parse(line);
//        if(is_builtin(arglist)){
//            run_command(arglist);
//        }
        run_command(arglist);
        fputs("minishell> ",stdout);
    }
    return 0;
}