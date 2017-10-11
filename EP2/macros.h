#ifndef __TYPES_H__
#define __TYPES_H__

#include <pthread.h>

#define P(x) pthread_mutex_lock(x)
#define V(x) pthread_mutex_unlock(x)
#define NUM_LANES 4

/* Simple types definition */
typedef enum { false, true } bool;

typedef enum { NONE, TOP, DOWN, TOPDOWN } Move;

typedef unsigned int u_int;

typedef unsigned long long int u_lint;

#endif
