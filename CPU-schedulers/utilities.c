/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 11/09/17
 *
 * Some util functions for timing, output and simple calculations.
 */

#include "utilities.h"

static FILE *outfile;
static int ctx_changes = 0;

/*
 * Function: val
 * --------------------------------------------------------
 * Returns the number in double of the seconds represented
 * for a given Time t
 *
 * @args t : Time struct (timeval)
 *
 * @return double representing the number of seconds
 */
double val(Time t){
    double total;
    total = (double)t.tv_sec;
    total +=  (double)t.tv_nsec*NANO_CONVERT;
    return total;
}
/*
 * Function: passed
 * --------------------------------------------------------
 * Returns the seconds passed since the timer was created
 *
 * @args self : the timer
 *
 * @return double representing seconds passed
 */
double passed(Timer self){
    Time current;
    clock_gettime(CLOCK_MONOTONIC_RAW, &current);
    double selfTotal = val(self->t);
    double currentTotal = val(current);
    return currentTotal - selfTotal;
}
/*
 * Function: value
 * --------------------------------------------------------
 * Same as val, but receives a Timer instead of time. Returns
 * the value of the seconds + nanoseconds in double.
 *
 * @args self : the timer
 *
 * @return value of seconds + nanoseconds in double.
 */
double value(Timer self){
    return val(self->t);
}

Timer new_Timer(){
    Timer self = emalloc(sizeof(struct timer_s));
    // CLOCK_MONOTONIC_RAW is the best for calculating elapsed time!
    clock_gettime(CLOCK_MONOTONIC_RAW, &(self->t));
    self->passed = &passed;
    self->value = &value;
    return self;
}
void destroy_Timer(Timer self){
    free(self);
}

void sleepFor(double dt){
    nanosleep(&(struct timespec){floor(dt),(long)((dt-floor(dt))/NANO_CONVERT)}, NULL);
}

void debugger(int EVENT_CODE, Process *p, int arg){
    if(DEBUG_MODE){
        switch (EVENT_CODE) {
            case ARRIVAL_EVENT:
                fprintf(stderr, "** %s (linha trace = %d) chegou no sistema! **\n",p->name, p->nLine);
                break;
            case RUN_EVENT:
                if(arg)
                    fprintf(stderr, "--> %s entrou na CPU %d!\n",p->name, arg);
                break;
            case EXIT_EVENT:
                if(arg)
                    fprintf(stderr, "<-- %s saiu da CPU %d!\n",p->name, arg);
                break;
            case END_EVENT:
                fprintf(stderr, "** %s (linha output = %d) finalizou no sistema! **\n",p->name, arg);
                break;
            case CONTEXT_EVENT:
                fprintf(stderr, "// Houve uma mudança de contexto! Total = %d //\n", ++ctx_changes);
                break;
        }
    }
    else if(EVENT_CODE == CONTEXT_EVENT)
        ++ctx_changes;
}

int get_ctx_changes() {
    return ctx_changes;
}

void open_outfile(const char* name) {
    outfile = efopen(name, "w");
}

void close_outfile() {
    fclose(outfile);
}

void write_outfile(const char* fmt, ...) {
    if (fmt) {
        va_list arglist;

        va_start(arglist, fmt);
        vfprintf(outfile, fmt, arglist);
        va_end(arglist);
    }
}

Process procdup(Process p){
    Process temp;
    temp.deadline = p.deadline;
    temp.dt = p.dt;
    temp.name = estrdup(p.name);
    temp.nLine = p.nLine;
    temp.pid = p.pid;
    temp.t0 = p.t0;
    return temp;
}
