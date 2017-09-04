#define _GNU_SOURCE
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
    //printf("         sleep %ld (sec) , %ld (nsec)\n",(long)floor(dt),(long)((dt-floor(dt))/NANO_CONVERT));
    nanosleep(&(struct timespec){floor(dt),(long)((dt-floor(dt))/NANO_CONVERT)}, NULL);
}

void debugger(int EVENT_CODE, Process p, int arg){
    if(DEBUG_MODE){
        switch (EVENT_CODE) {
            case ARRIVAL_EVENT:
                fprintf(stderr, "** %s (linha trace = %d) chegou no sistema! **\n",p.name, p.nLine);
                break;
            case RUN_EVENT:
                fprintf(stderr, "--> %s entrou na CPU %d!\n",p.name, sched_getcpu());
                break;
            case EXIT_EVENT:
                fprintf(stderr, "<-- %s saiu da CPU %d!\n",p.name, sched_getcpu());
                break;
            case END_EVENT:
                fprintf(stderr, "** %s (linha output = %d) finalizou no sistema! **\n",p.name, arg);
                break;
            case CONTEXT_EVENT:
                fprintf(stderr, "// Houve uma mudança de contexto! Total = %d //\n", ++ctx_changes);
                break;
        }
    }
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
