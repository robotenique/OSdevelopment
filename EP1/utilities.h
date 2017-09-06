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
typedef enum {ARRIVAL_EVENT, RUN_EVENT, EXIT_EVENT, END_EVENT, CONTEXT_EVENT} EVENTS;
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "error.h"
#include "process.h"
#include "minPQ.h"

bool sigval; //TODO: remove all instances of this thing (after statistics, of course >:) )

typedef struct timespec Time;
struct timer_s{
    Time t;
    double(*passed)(struct timer_s*);
    double(*value)(struct timer_s*);
};
typedef struct timer_s* Timer;

bool DEBUG_MODE;

// to calculate the deadline!
typedef struct deadlineC{
    double realFinished;
    double deadline;
    // The time a process had to wait to be executed for the 1st time
    double waitTime;
}deadlineC;

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
void debugger(int, Process*, int);

/*
 * Function: open_outfile
 * --------------------------------------------------------
 * Open outfile to print statistics about processes
 *
 * @args name : name of the file
 *
 * @return
 */
void open_outfile(const char*);

/*
 * Function: close_outfile
 * --------------------------------------------------------
 * Close outfile
 *
 * @args
 *
 * @return
 */
void close_outfile();

/*
 * Function: write_outfile
 * --------------------------------------------------------
 * Write a formated string to outfile
 *
 * @args fmt : formated string
 *       ... : args to format string
 *
 * @return
 */
void write_outfile(const char*, ...);
/*
 * Function: procdup
 * --------------------------------------------------------
 * Duplicate a process and return it
 *
 * @args p : the process
 *
 * @return a copy of the process
 */
Process procdup(Process);

int get_ctx_changes();

#endif
