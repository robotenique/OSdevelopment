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

#include <readline/readline.h>
#include <readline/history.h>
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
    while (1) {
        // TODO Print working directory
        inpt = readline(">>> ");
        add_history(inpt);

        // TODO Split line and execute command
        // Test lines ---------//
        printf("%s\n", inpt);  //
        // --------------------//

        // TODO Implement chown and date

        free(inpt);
    }
    return 0;
}
