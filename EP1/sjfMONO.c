/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 11/09/17
 *
 * Shortest Job First scheduler implementation.
 */
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "error.h"
#include "process.h"
#include "minPQ.h"
#include "utilities.h"

#define CPU_CORE 1

static Timer timer;
int cmpSJF(Process, Process);
void *execProcess(void *proc);

/*
 * Function: schedulerSJF
 * --------------------------------------------------------
 * Receives an array of processes to run (the pQueue), and the name
 * of the file to write the informations.
 * The algorithm uses a MinPQ to sort by dt. It gets all processes
 * in the pool that have arrived, then put then into the MinPQ.
 * Then, run the min dt process of the MinPQ. After this, checks again
 * the pool to see if another processes has arrived. If true, add the
 * new processes into the MinPQ, and procedes the algorithm (run the
 * min dt process of the MinPQ), until there's no process left in the
 * MinPQ AND in the pool.
 * If there's no process in the MinPQ, sleeps until another process
 * arrives, then proceed the algorithm.
 *
 * @args  pQueue :  A ProcArray with the processes to run
 *        outfile : The name of the file to write the information
 *
 * @return
 */
void schedulerSJF(ProcArray pQueue){
    int sz = pQueue->i + 1;
    int outLine = 0;
    MinPQ pPQ = new_MinPQ(&cmpSJF);
    Process curr = pQueue->v[pQueue->nextP++];
    timer = new_Timer();
    // Sleep until the first process arrives at the cpu
    sleepFor(curr.t0);
    debugger(ARRIVAL_EVENT, &curr, 0);
    pPQ->insert(pPQ, &curr);
    while(pQueue->nextP < pQueue->i || !pPQ->isEmpty(pPQ)){
        double tNow = timer->passed(timer);
        int i;
        if(pPQ->isEmpty(pPQ)){
            // If there isn't processes to run, sleep until the next t0
            sleepFor(pQueue->v[pQueue->nextP].t0 - tNow);
            tNow = timer->passed(timer);
        }
        // Insert all the arrived processes into the MinPQ
        for(i = pQueue->nextP; i < pQueue->i &&  pQueue->v[i].t0 <= tNow; i++){
            debugger(ARRIVAL_EVENT, &(pQueue->v[i]), 0);
            pPQ->insert(pPQ, &pQueue->v[i]);
        }
        pQueue->nextP = i;
        if(!pPQ->isEmpty(pPQ)){
            // Run the min dt process
            curr = *pPQ->delMin(pPQ);
            debugger(CONTEXT_EVENT, NULL, 0);
            pthread_create(&curr.pid, NULL, &execProcess, &curr);
            pthread_join(curr.pid, NULL);
            debugger(END_EVENT, &curr, ++outLine);
            write_outfile("%s %lf %lf\n",curr.name, timer->passed(timer), timer->passed(timer) - curr.t0);
        }
    }
    // In SJF there's no context switch...
    write_outfile("%d\n", get_ctx_changes());
    destroy_Timer(timer);
    destroy_MinPQ(pPQ);
}

/*
 * Function: cmpSJF
 * --------------------------------------------------------
 * Priority function to use in the MinPQ
 *
 * @args a : the first Process
 *       b : the second process
 *
 * @return 1 if a > b, -1 if b < a, 0 otherwise
 */
int cmpSJF(Process a, Process b){
    if (a.dt < b.dt)
        return -1;
    else if (a.dt > b.dt)
        return 1;
    return 0;
}

/*
 * Function: execProcess
 * --------------------------------------------------------
 * Simulates a process executing (Sleeps for the dt of the process)
 *
 * @args proc : a pointer to a process...
 *
 * @return
 */
void *execProcess(void *proc){
    Process *p = (Process *)proc;
    debugger(RUN_EVENT, p, CPU_CORE);
    sleepFor(p->dt);
    debugger(EXIT_EVENT, p, CPU_CORE);
    pthread_exit(NULL);
}
