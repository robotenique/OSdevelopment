#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#define NANO_CONVERT 1e-9
#define ARRIVAL_EVENT 0
#define RUN_EVENT 1
#define EXIT_EVENT 2
#define END_EVENT 3
#define CONTEXT_EVENT 4
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "error.h"
#include "process.h"
#include "minPQ.h"


typedef struct timespec Time;
struct timer_s{
    Time t;
    double(*passed)(struct timer_s*);
    double(*value)(struct timer_s*);
};
typedef struct timer_s* Timer;

Timer new_Timer();
void destroy_Timer();
void sleepFor(double);
void debugger(int, Process, int);

#endif
