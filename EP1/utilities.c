#define _GNU_SOURCE
#include "utilities.h"

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
                fprintf(stderr, "// Houve uma mudança de contexto! Total = %d //\n", arg);
                break;
        }
    }
}
