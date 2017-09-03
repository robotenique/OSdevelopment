#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "error.h"
#include "process.h"

#define NANO_CONVERT 0.0000000001
/* ----------- #TODO: PUT THE TIMER IN A IN A SEPARATE FILE ------------- */
typedef struct timespec Time;
struct timer_s{
    Time t;
    double(*passed)(struct timer_s*);
    double(*value)(struct timer_s*);
};
typedef struct timer_s* Timer;

double val(Time t){
    double total;
    total = (double)t.tv_sec;
    total +=  (double)t.tv_nsec*NANO_CONVERT;
    return total;
}
double passed(Timer self){
    Time current;
    clock_gettime(CLOCK_MONOTONIC, &current);
    double selfTotal = val(self->t);
    double currentTotal = val(current);
    return currentTotal - selfTotal;
}
double value(Timer self){
    return val(self->t);
}
Timer new_Timer(){
    Timer self = emalloc(sizeof(struct timer_s));
    clock_gettime(CLOCK_MONOTONIC, &(self->t));
    self->passed = &passed;
    self->value = &value;
    return self;
}
void destroy_Timer(Timer self){
    free(self);
}
/* ----------- -------------------------------------------- ------------- */





void schedulerSJF(ProcArray readyJobs, char *outfile){
    Timer timer = new_Timer();
    printf("Olar tAtual = %lf \n",timer->value(timer));
    Time t;
    t.tv_sec = 4;
    t.tv_nsec = 0;
    nanosleep(&t, NULL);
    printf("Olar tAtual = %lf \n",timer->passed(timer));

}
