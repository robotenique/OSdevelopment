/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 11/09/17
 *
 * String buffer implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "error.h"

Buffer *buffer_create(){
	Buffer *buff;
	buff = emalloc(sizeof(Buffer));
	buff->data = emalloc(1024 * sizeof(char));
	for (int i = 0; i < 1024; i++)
		buff->data[i] = '\0';
	buff->i = 0;
	buff->n = 1024;
	return buff;
}

void buffer_destroy(Buffer *B){
	free(B->data);
	free(B);
	return;
}

void buffer_reset(Buffer *B){
	for (int i = 0; i < B->i; i++)
		B->data[i] = '\0';
	B->i = 0;
	return;
}

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
