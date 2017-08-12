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
#include <errno.h>
#include <time.h>
#include "error.h"
#include "buffer.h"

#include <sys/wait.h>
#include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>

char * buildShellString();
char **splitString(const char*);
char *getName(const char*);
void printDate();

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
    char *fshell;
    char **largv;
    char *path;

    set_prog_name("ep1sh");

    while (1) {
        fshell = buildShellString();
        inpt = readline(fshell);
        add_history(inpt);
        largv = splitString(inpt);

        if (largv) {
            path = largv[0];
            largv[0] = getName(path);
            if (!strcmp(largv[0], "exit")){
                for (; *largv != NULL; largv++)
                    free(*largv);
                free(inpt);
                free(fshell);
                exit(0);
            }
            else if (!strcmp(largv[0], "date")) {
                pid_t child;
                int status;
                if ((child = fork()) == 0) {
                    printDate();
                    exit(0);
                }
                else
                    waitpid(child, &status, 0);
            }
            // TODO Implement chown
            else {
                // TODO Maybe redirect signals like ^C and ^Z to child process
                pid_t child;
                int status;
                if ((child = fork()) == 0) {
                    execvp(path, largv);
                    die("cannot find: %s\n", path);
                }
                else
                    waitpid(child, &status, 0);
            }

            for (; *largv != NULL; largv++)
                free(*largv);
            free(path);
        }

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

    fshell[0] = '[';
    strcpy(fshell + 1, buf);
    fshell[size - 1] = '\0';
    fshell[size - 2] = ' ';
    fshell[size - 3] = '$';
    fshell[size - 4] = ']';

    free(buf);
    return fshell;
}

/*
 * Function: splitString
 * --------------------------------------------------------
 * Receive a string and split it into words, using any whitespace character
 * as divisor (unless it's preceeded with a backward slash character),
 * and returns the result as a NULL ended array of null-terminated strings.
 *
 * @args string : const char*
 *
 * @return a pointer to the array or NULL if string is entirely made of
 *         whitespaces or the string is NULL.
 */
char **splitString(const char* string) {
    Buffer *buff;
    char **argv;
    int i, j, mem;
    int size = strlen(string);
    int state = 0;
    int words = 0;
    if (!string)
        return (char **)NULL;

    // Skip trailing whitespaces
    for (i = 0; i < size && whitespace(string[i]); i++);
    if (i == size)
        return (char **)NULL;
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
    argv = emalloc((words + 1)*sizeof(char *));
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
                argv[j] = estrdup(buff->data);
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
        argv[j] = estrdup(buff->data);
        j++;
    }

    argv[j] = (char *)NULL;

    buffer_destroy(buff);
    return argv;
}

/*
 * Function: getName
 * --------------------------------------------------------
 * Receive the path to a program and returns the program name.
 *
 * @args path : const char*
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
    name[buff->i] = '\0';
    buffer_destroy(buff);
    return name;
}

/*
 * Function: printDate
 * --------------------------------------------------------
 * Emulate date command without flags
 *
 * @args none
 *
 * @return void
 */
void printDate() {
    char *const wdays[] = {"dom", "seg", "ter", "qua", "qui", "sex", "sáb"};
    char *const months[] = {"jan", "fev", "mar", "abr", "mai", "jun", "jul",
                            "ago", "set", "out", "nov", "dez"};
    struct tm *cal;
    char *s;
    time_t epoch;

    time(&epoch);
    cal = localtime(&epoch);

    s = emalloc(20*sizeof(char));
    strftime(s, 21, "%d %H:%M:%S %Z %Y", cal);
    printf("%s %s %s\n", wdays[cal->tm_wday], months[cal->tm_mon], s);

    free(s);
    return;
}
