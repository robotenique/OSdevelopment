/*
  error.h
  Error-handling routines.
  This code is an adaptation of the module provided during
  our MAC216 course.
  https://www.ime.usp.br/~fmario/cursos/mac216-15/
*/

#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdlib.h>

/*
  Set the program name to format error messages.
*/
void set_prog_name(const char *name);

/*
  Return error message set.
*/
const char *get_error_msg();

/*
  Set error message to msg, formatted with arguments as in sprintf.
*/
void set_error_msg(const char *msg, ...);

/*
  Print error message in stderr.

  If pointer msg is NULL, then the error message that has been set is
  used; otherwise the message given is used, formatted with arguments
  as in sprintf.

  If the message ends in a colon (:) then the system's error message is
  concatenated, in case errno is nonzero; otherwise the colon is
  removed and replaced by a period.
*/
void print_error_msg(const char *msg, ...);

/*
  Print error message like print_error_msg and crashes the program.
*/
void die(const char *msg, ...);

/*
  Like malloc, but crashes the program with an error message on
  failure.
*/
void *emalloc(size_t size);

/*
  Like strdup, but crashes the program with an error message on
  failure.
*/
char *estrdup(const char *s);

#endif
