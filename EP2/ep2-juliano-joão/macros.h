/*
 * @author: João Gabriel Basi Nº USP: 9793801
 * @author: Juliano Garcia de Oliveira Nº USP: 9277086
 *
 * MAC0422
 * 16/10/17
 *
 * A header with some macros
 */

#ifndef __TYPES_H__
#define __TYPES_H__

#include <pthread.h>

#define P(x) pthread_mutex_lock(x)
#define V(x) pthread_mutex_unlock(x)
#define NUM_LANES 10

/* Simple types definition */
typedef enum { false, true } bool;

typedef unsigned int u_int;

typedef unsigned long long int u_lint;

#endif
