/*
 * @author: João Gabriel Basi Nº USP: 9793801
 * @author: Juliano Garcia de Oliveira Nº USP: 9277086
 *
 * MAC0422
 * 16/10/17
 *
 * Header for the debugging functions
 */
#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

#include "bikeStructures.h"

#define RESET "\x1b[0m"

void debug_road();
void random_initialize(u_int);
void debug_buffer(Buffer);

/*
 * Function: print_prog_name
 * --------------------------------------------------------
 * Prints out the ASCII art
 *
 * @args
 *
 * @return
 */
void print_prog_name();
const char *get_color(u_int);
#endif
