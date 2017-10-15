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

#define RESET "\x1b[0m"

/*
 * Function: debug_road
 * --------------------------------------------------------
 * Prints out a formatted road with bikers' ids
 *
 * @args
 *
 * @return
 */
void debug_road();

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
#endif
