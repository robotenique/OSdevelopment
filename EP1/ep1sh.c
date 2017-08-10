/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 11/09/17
 *
 * A custom Shell made in C. Part 1 of EP1.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "error.h"


#include <readline/readline.h>
#include <readline/history.h>

char * buildShellString();

/*
 * Function: main
 * --------------------------------------------------------
 * Executes the Shell and awaits the user commands
 *
 * @args
 *
 * @return default
 */
int main(int argc, char const *argv[]) {
    char *inpt;
    char *fshell = buildShellString();
    while (1) {
        // TODO Print working directory
        inpt = readline(fshell);
        add_history(inpt);
        if(!strcmp(inpt,"exit")){
            free(inpt);
            free(fshell);
            exit(0);
        }

        // TODO Split line and execute command
        // Test lines ---------//
        printf("%s\n", inpt);  //
        // --------------------//

        // TODO Implement chown and date

        free(inpt);
    }
    return 0;
}

/*
 * Function: buildShellString
 * --------------------------------------------------------
 * Get the current working directory, and build the shell
 * initial string in the form '[x]$ ' as required.
 *
 * @args
 *
 * @return the formmated string for the shell input
 */
char * buildShellString(){
    long size = pathconf(".", _PC_PATH_MAX);
    char *buf = (char *)emalloc(size);
    getcwd(buf, size);
    size = strlen(buf) + 5;
    char *fshell = emalloc(size);
    fshell[0]='[';
    fshell[size - 1] = 0;
    fshell[size - 2] = ' ';
    fshell[size - 3] = '$';
    fshell[size - 4] = ']';

    int i;
    int j;
    for (i = 1, j = 0; i < size - 4; fshell[i++] = buf[j++]);
    free(buf);
    return fshell;
}
