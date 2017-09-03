/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 11/09/17
 *
 * Header for utilities. Defines some useful constants
 * and event codes for the debugger
 */
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


/*
 * Function: new_Timer
 * --------------------------------------------------------
 * Creates a new timer and returns it
 *
 * @args
 *
 * @return  a new timer
 */
Timer new_Timer();
/*
 * Function: destroy_Timer
 * --------------------------------------------------------
 * Destroy a timer, freeing memory
 *
 * @args timer : the timer to destroy
 *
 * @return
 */
void destroy_Timer(Timer);
/*
 * Function: sleepFor
 * --------------------------------------------------------
 * Sleeps for dt seconds
 *
 * @args dt : seconds to sleep
 *
 * @return
 */
void sleepFor(double);
/*
 * Function: debugger
 * --------------------------------------------------------
 * Print to stderr debug information, which information and
 * format is specified mainly by the EVENT_CODE argument.
 * It always receives a process.
 *
 * @args EVENT_CODE : code of the event.
 *       p : a process
 *       arg : an integer, arbitrary argument. Used in specific
 *             instances of the debugger, like the END_EVENT
 *
 * @return
 */
void debugger(int, Process, int);

#endif
