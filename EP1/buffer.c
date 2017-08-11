#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "error.h"

Buffer *buffer_create(){
	Buffer *buff;
	buff = emalloc(sizeof(Buffer));
	buff->data = emalloc(1024 * sizeof(char));
	buff->i = 0;
	buff->n = 1024;
	return buff;
}

/*
  Destroy a buffer.
*/
void buffer_destroy(Buffer *B){
	free(B->data);
	free(B);
	return;
}

/*
  Reset buffer, eliminating contents.
*/
void buffer_reset(Buffer *B){
	for (int i = 0; i < B->i; i++)
		B->data[i] = 0;
	B->i = 0;
	return;
}

/*
  Add a bool b to the end of the buffer.
*/
void buffer_push_back(Buffer *B, char c){
	char *Vaux;
	B->data[B->i] = c;
	(B->i)++;
	if (B->i == B->n){
		Vaux = emalloc(2 * B->n * sizeof(char));
		for (int i = 0; i < B->n; i++)
			Vaux[i] = B->data[i];
		free(B->data);
		B->data = Vaux;
		B->n *= 2;
	}
	return;
}

/*
  Read a line (i.e., reads up to a newline '\n' character or the
  end-of-file) from the input file and places it into the given
  buffer, including the newline character if it is present. The buffer
  is resetted before the line is read.

  Returns the number of characters read; in particular, returns ZERO
  if end-of-file is reached before any characters are read.
*/
int read_line(FILE *input, Buffer *B){
	char c = fgetc(input);
	int count_char = 0;
	buffer_reset(B);
	while (c != EOF && c != '\n'){
		buffer_push_back(B, c);
		count_char++;
		c = fgetc(input);
	}
	if (c == '\n'){
		buffer_push_back(B, c);
		count_char++;
	}
	return count_char;
}
