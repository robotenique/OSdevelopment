#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "error.h"
#include "process.h"
#include "minPQ.h"
#include "utilities.h"
#define IDLE_STATE 2

static pthread_mutex_t gmtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_t *CPU;
static long numCPU;
static bool finished;
static bool busy = false;
static deadlineC *deadArray;
static Timer timer;
static unsigned int runningThreads;

int cmpSJF(Process, Process);
bool availableCPU();
void startProcess(int , Process *);

bool availableCPU(){
    // Returns if there's a CPU available to run a process
    return runningThreads != numCPU;
}


void schedulerSJFMultithread(ProcArray pQueue){
    runningThreads = 0;
    numCPU = sysconf( _SC_NPROCESSORS_ONLN);
    CPU = emalloc(sizeof(pthread_t)*numCPU);
    for (int i = 0; i < numCPU; i++)
        CPU[i] = IDLE_STATE;

    int sz = pQueue->i + 1;
    deadArray = emalloc(sizeof(deadlineC)*sz);
    int outLine = 1;
    MinPQ pPQ = new_MinPQ(&cmpSJF);
    Process curr = pQueue->v[pQueue->nextP++];
    timer = new_Timer();
    // Sleep until the first process arrives at the cpu
    sleepFor(curr.t0);
    debugger(ARRIVAL_EVENT, curr, 0);
    pPQ->insert(pPQ, &curr);
    while(pQueue->nextP < pQueue->i || !pPQ->isEmpty(pPQ)){
        double tNow = timer->passed(timer);
        int i;
        if(pPQ->isEmpty(pPQ)){
            printf("Scheduler is going to sleep for %lfs...", pQueue->v[pQueue->nextP].t0 - tNow);
            sleepFor(pQueue->v[pQueue->nextP].t0 - tNow);
            tNow = timer->passed(timer);
        }
        // Insert all the arrived processes into the MinPQ
        for(i = pQueue->nextP; i < pQueue->i &&  pQueue->v[i].t0 <= tNow; i++){
            debugger(ARRIVAL_EVENT, pQueue->v[i], 0);
            pPQ->insert(pPQ, &pQueue->v[i]);
        }
        pQueue->nextP = i;
        pthread_mutex_lock(&gmtx);
        // run all processes that we have in the available CPUS
        for(int k = 0; k < numCPU && !pPQ->isEmpty(pPQ); k++)
            if(CPU[k] == IDLE_STATE)
                startProcess(k, pPQ->delMin(pPQ));

        /* Only proceed the scheduling process if there's a CPU available.
         * Since we are simulating processes in different CPUs, the schedulerSJF
         * is itself another process. So we continue to run it if there isn't a CPU
         * available for the scheduler itself!
        */
        if (!availableCPU())
          pthread_cond_wait(&cond, &gmtx);
    }
    pthread_mutex_unlock(&gmtx);
}

void startProcess(int nCPU, Process *p){
    
}


int cmpSJF(Process a, Process b){
    if (a.dt < b.dt)
        return -1;
    else if (a.dt > b.dt)
        return 1;
    return 0;
}
