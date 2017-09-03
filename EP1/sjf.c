#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "error.h"
#include "process.h"
#include "minPQ.h"

#define NANO_CONVERT 1e-9
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

void sleepFor(double dt){
    // IDLE for time dt (in Seconds)
    printf("Dormindo por %ld (sec)  e %ld (nsec)\n",(long)floor(dt),(long)((dt-floor(dt))/NANO_CONVERT));
    nanosleep(&(struct timespec){floor(dt),(long)((dt-floor(dt))/NANO_CONVERT)}, NULL);

}


/* ----------- -------------------------------------------- ------------- */

int cmpSJF(Process a, Process b){
    if (a.dt < b.dt)
        return -1;
    else if (a.dt > b.dt)
        return 1;
    return 0;
}

void schedulerSJF(ProcArray readyJobs, char *outfile){
    printf("OLar...\n");
    //Processes to arrive
    ProcArray rj = readyJobs;
    // Processes to run
    MinPQ pPQ = new_MinPQ(&cmpSJF);
    // Get the first process
    Process curr = rj->v[rj->nextP++];
    // Initialize the timer
    Timer timer = new_Timer();
    // Sleep until the first process arrives at the cpu
    if(curr.t0 >= 1)
        sleepFor(curr.t0);
    pPQ->insert(pPQ, curr);
    while(rj->nextP < rj->size){
        double tNow = timer->passed(timer);
        int i;
        for(i = rj->nextP; i < rj->size &&  rj->v[i].t0 <= tNow; i++)
            pPQ->insert(pPQ, rj->v[i]);
        rj->nextP = i;
        curr = pPQ->delMin(pPQ);

    }
    while(!pPQ->isEmpty(pPQ)){
        // Run the remaining processes
    }


}
