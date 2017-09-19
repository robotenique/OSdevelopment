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
 * Function: buffer_create
 * --------------------------------------------------------
 * Create and return a new and empty buffer.
 *
 * @args none
 *
 * @return a pointer to the created buffer
 */
Buffer *buffer_create();

/*
  Destroy a buffer.
*/
/*
 * Function: buffer_destroy
 * --------------------------------------------------------
 * Destroy a buffer.
 *
 * @args B : Buffer*
 *
 * @return void
 */
void buffer_destroy(Buffer *B);

/*
 * Function: buffer_reset
 * --------------------------------------------------------
 * Reset buffer, eliminating contents.
 *
 * @args B : Buffer*
 *
 * @return void
 */
void buffer_reset(Buffer *B);

/*
 * Function: buffer_push_back
 * --------------------------------------------------------
 * Add a char c to the end of the buffer.
 *
 * @args B : Buffer*
 *       c : char
 *
 * @return void
 */
void buffer_push_back(Buffer *B, char c);

/*
 * Function: read_line
 * --------------------------------------------------------
 * Read a line (up to a newline '\n' character or the end-of-file) from
 * the input file and places it into the given buffer, including the
 * newline character if it is present. The buffer is resetted before
 * the line is read.
 *
 * @args input : FILE*
 *       B : Buffer*
 *
 * @return number of character read
 */
int read_line(FILE *input, Buffer *B);

#endif
