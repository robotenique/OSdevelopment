/*
 * @author: João Gabriel Basi
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
#include "buffer.h"

#include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>

typedef struct path_argv_pair {
    char **argv;
    char *path;
} Pair;

char * buildShellString();
Pair *splitString(const char*);
char *getName(const char*);

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
    Pair *pair;
    char *inpt;
    char *fshell;
    int i;
    while (1) {
        fshell = buildShellString();
        inpt = readline(fshell);
        add_history(inpt);
        pair = splitString(inpt);
        if (pair) {
            if(!strcmp(pair->argv[0], "exit")){
                for (i = 0; pair->argv[i] != NULL; i++)
                    free(pair->argv[i]);
                free(pair->path);
                free(pair);
                free(inpt);
                free(fshell);
                exit(0);
            }
            else {
                // TODO Wait until child process end to loop again
                // TODO Redirect signals like ^C and ^Z to child process
                pid_t child;
                if ((child = fork()) == 0)
                    // TODO It gets buggy when receiving something not
                    // executable, like "cow", as path
                    execve(pair->path, pair->argv, __environ);
            }
            for (i = 0; pair->argv[i] != NULL; i++)
                free(pair->argv[i]);
            free(pair->path);
            free(pair);
        }

        // TODO Implement chown and date

        free(fshell);
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

/*
 * Function: splitString
 * --------------------------------------------------------
 * Gets a string and split it into words, using any whitespace character
 * as divisor (unless it's preceeded with a backward slash character),
 * and returns the result as a NULL ended array of null-terminated strings.
 *
 * @args string : string
 *
 * @return a pointer to the array or NULL if string is entirely made of
 *         whitespace or string is NULL.
 */
Pair *splitString(const char* string) {
    Buffer *buff;
    Pair *pair;
    int i, j, mem;
    int size = strlen(string);
    int state = 0;
    int words = 0;
    if (!string)
        return (Pair *)NULL;

    // Removes trailing whitespaces
    for (i = 0; i < size && whitespace(string[i]); i++);
    if (i == size)
        return (Pair *)NULL;
    mem = i;

    // Count words
    for (; i < size; i++) {
        if (string[i] == '\\') {
            state = 1;
            i++;
        }
        else if (!state && !whitespace(string[i])) {
            state = 1;
            words++;
        }
        else if (whitespace(string[i]))
            state = 0;
    }

    // Add words to array
    pair = emalloc(sizeof(Pair));
    pair->argv = emalloc((words + 1)*sizeof(char *));
    buff = buffer_create();
    for (i = mem, j = 0, state = 0; i < size; i++) {
        if (string[i] == '\\') {
            buffer_push_back(buff, string[i]);
            state = 1;
            if (i++ < size)
                buffer_push_back(buff, string[i]);
            continue;
        }
        if (whitespace(string[i])) {
            if (state) {
                pair->argv[j] = buffer_to_string(buff);
                buffer_reset(buff);
                j++;
            }
            state = 0;
        }
        else {
            buffer_push_back(buff, string[i]);
            state = 1;
        }
    }
    if (buff->i) {
        pair->argv[j] = buffer_to_string(buff);
        j++;
    }

    pair->path = pair->argv[0];
    pair->argv[0] = getName(pair->path);
    pair->argv[j] = (char *)NULL;

    buffer_destroy(buff);
    return pair;
}

/*
 * Function: getName
 * --------------------------------------------------------
 * Gets the path to a program and returns the program name.
 *
 * @args path : string
 *
 * @return program name
 */
char *getName(const char* path) {
    Buffer *buff;
    char *name;
    int i, j;
    buff = buffer_create();
    for (i = strlen(path) - 1; i >= 0 && path[i] != '/'; i--)
        buffer_push_back(buff, path[i]);
    name = emalloc((buff->i + 1)*sizeof(char));
    for (i = buff->i - 1, j = 0; i >= 0; i--, j++)
        name[j] = buff->data[i];
    name[buff->i] = 0;
    return name;
}
