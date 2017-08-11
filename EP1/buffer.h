#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdio.h>

// Buffer struct.
typedef struct buffer_s {
    char *data;
    // Buffer max. size and first free position.
    int n, i;
} Buffer;

/*
  Create and return a new and empty buffer.
*/
Buffer *buffer_create();

/*
  Destroy a buffer.
*/
void buffer_destroy(Buffer *B);

/*
  Reset buffer, eliminating contents.
*/
void buffer_reset(Buffer *B);

/*
  Add a char c to the end of the buffer.
*/
void buffer_push_back(Buffer *B, char c);

/*
  Read a line (i.e., reads up to a newline '\n' character or the
  end-of-file) from the input file and places it into the given
  buffer, including the newline character if it is present. The buffer
  is resetted before the line is read.

  Returns the number of characters read; in particular, returns ZERO
  if end-of-file is reached before any characters are read.
*/
int read_line(FILE *input, Buffer *B);

/*
  Return buffer content as a string
*/
char *buffer_to_string(Buffer* B);

#endif
