/*
 * @author: João Gabriel Basi Nº USP: 9793801
 * @author: Juliano Garcia de Oliveira Nº USP: 9277086
 *
 * MAC0422
 * 16/10/17
 *
 * Header for the randomizer module
 */
#ifndef __RANDOMIZER_H__
#define __RANDOMIZER_H__

#include "bikeStructures.h"
/*
 * Function: randint
 * --------------------------------------------------------
 * Generates a random unsigned integer n, such that
 * a <= n < b, using C random generator.
 *
 * @args  a :  the lower bound (inclusive)
 * @args  b :  the upper bound (exclusive)
 *
 * @return  A number between a (inclusive) and b(exclusive)
 */
u_int randint(u_int, u_int);

/*
 * Function: event
 * --------------------------------------------------------
 * Runs a probabilistic event and returns true on sucess,
 * false on fail.
 *
 * @args  probability :  the prob. of the event (from 0 to 1)
 *
 * @return  a boolean representing sucess of fail
 */
bool event(double);

#endif
