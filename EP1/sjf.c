#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "error.h"
#include "process.h"
#include "minPQ.h"
#include "utilities.h"

int cmpSJF(Process a, Process b){
    if (a.dt < b.dt)
        return -1;
    else if (a.dt > b.dt)
        return 1;
    return 0;
}


void *runSJF(void *proc){
    Process p = (*(Process *)proc);
    debugger(RUN_EVENT, p, 0);
    sleepFor(p.dt);
    debugger(EXIT_EVENT, p, 0);
    pthread_exit(NULL);
}

void schedulerSJF(ProcArray readyJobs, char *outfile){
    FILE* out = efopen (outfile, "w");
    int outLine = 1;
    ProcArray rj = readyJobs;
    MinPQ pPQ = new_MinPQ(&cmpSJF);
    Process curr = rj->v[rj->nextP++];
    Timer timer = new_Timer();
    // Sleep until the first process arrives at the cpu
    sleepFor(curr.t0);
    debugger(ARRIVAL_EVENT, curr, 0);
    pPQ->insert(pPQ, curr);

    while(rj->nextP < rj->i || !pPQ->isEmpty(pPQ)){
        double tNow = timer->passed(timer);
        int i;
        if(pPQ->isEmpty(pPQ)){
            sleepFor(rj->v[rj->nextP].t0 - tNow);
            tNow = timer->passed(timer);
        }
        for(i = rj->nextP; i < rj->i &&  rj->v[i].t0 <= tNow; i++){
            debugger(ARRIVAL_EVENT, rj->v[i], 0);
            pPQ->insert(pPQ, rj->v[i]);
        }
        rj->nextP = i;
        if(pPQ->isEmpty(pPQ))   continue;
        curr = pPQ->delMin(pPQ);
        pthread_create(&curr.pid, NULL, &runSJF, &curr);
        pthread_join(curr.pid, NULL);
        debugger(END_EVENT, curr, outLine++);
        fprintf(out, "%s %lf %lf\n",curr.name, timer->passed(timer), timer->passed(timer) - curr.t0);
    }
    // In SJF there's no context switch...
    fprintf(out, "%d\n",0);
}
