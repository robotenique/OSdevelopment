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
#define IDLE_STATE 0
#define RUNNING_STATE 1

//TODO: remove USELESS printf all across the EP

typedef struct output{
    char *name;
    double tf;
    double treal;
    int outN;
} OutputData;


static OutputData **outInfo;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t schedMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static unsigned int *CPU;
static pthread_t *coreCPU;
static long numCPU;
static deadlineC *deadArray;
static Timer timer;
static Process **pool;
static unsigned int idleCPU;
static int outLine;


int cmpSJFThreaded(Process, Process);
void startProcess(int);
void *processRoutine(void *);
void writeOutput(int);


OutputData *new_output(char *name, double tf, double treal, int outN){
    OutputData *ret = emalloc(sizeof(OutputData));
    ret->name = estrdup(name);
    ret->tf = tf;
    ret->treal = treal;
    ret->outN = outN;
    return ret;
}


void schedulerSJFMultithread(ProcArray pQueue){
    int sz = pQueue->i + 1;
    numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    outInfo = emalloc(sizeof(OutputData*)*pQueue->i);
    deadArray = emalloc(sizeof(deadlineC)*sz);
    coreCPU = emalloc(sizeof(pthread_t)*numCPU);
    CPU = emalloc(sizeof(unsigned int)*numCPU);
    pool = emalloc(sizeof(Process *)*numCPU);
    for (int i = 0; i < numCPU; i++)
        CPU[i] = IDLE_STATE;
    for(int i = 0; i < sz - 1; i++)
        outInfo[i] = NULL;
    outLine = 0;
    MinPQ pPQ = new_MinPQ(&cmpSJFThreaded);
    Process curr = pQueue->v[pQueue->nextP++];
    timer = new_Timer();
    idleCPU = numCPU;

    // Sleep until the first process arrives at the cpu
    sleepFor(curr.t0);
    debugger(ARRIVAL_EVENT, &curr, 0);
    pPQ->insert(pPQ, &curr);
    pthread_mutex_lock(&schedMutex);
    while(pQueue->nextP < pQueue->i || !pPQ->isEmpty(pPQ)){
        double tNow = timer->passed(timer);
        int i;
        if(pPQ->isEmpty(pPQ)){
            //printf("Scheduler is going to sleep for %lfs...", pQueue->v[pQueue->nextP].t0 - tNow);
            sleepFor(pQueue->v[pQueue->nextP].t0 - tNow);
            tNow = timer->passed(timer);
        }
        // Insert all the arrived processes into the MinPQ
        for(i = pQueue->nextP; i < pQueue->i &&  pQueue->v[i].t0 <= tNow; i++){
            debugger(ARRIVAL_EVENT, &pQueue->v[i], 0);
            pPQ->insert(pPQ, &pQueue->v[i]);
        }
        pQueue->nextP = i;
        pthread_mutex_lock(&mutex);
            // run all processes that we have in the available CPUS
            for(int k = 0; k < numCPU && !pPQ->isEmpty(pPQ); k++)
                if(CPU[k] == IDLE_STATE){
                    CPU[k] = RUNNING_STATE;
                    pool[k] = pPQ->delMin(pPQ);
                    idleCPU--;
                    debugger(CONTEXT_EVENT, NULL, 0);
                    startProcess(k);
                }
        pthread_mutex_unlock(&mutex);

        /* Only proceed the scheduling process if there's a CPU available.
         * Since we are simulating processes in different CPUs, the schedulerSJF
         * is itself another process. So we continue to run it if there isn't a CPU
         * available for the scheduler itself!
        */
        if(idleCPU == 0)
            pthread_cond_wait(&cond, &schedMutex);
    }
    pthread_mutex_unlock(&schedMutex);
    // Wait for all processes to finish their execution
    for(int c = 0; c < numCPU; c++)
        if(CPU[c] != IDLE_STATE)
            pthread_join(coreCPU[c],NULL);
    writeOutput(sz - 1);
    write_outfile("%d\n", get_ctx_changes());
    destroy_Timer(timer);
    destroy_MinPQ(pPQ);
    free(coreCPU);
    free(CPU);
    free(pool);

    // TODO: remove deadline statistics later
    int counter = 0;
    double avgDelay = 0;
    printf("\n\n");
    for (int i = 1; i < sz; i++) {
        deadlineC dc = deadArray[i];
        printf("Processo da linha %d : tReal = %lf , deadline = %lf\n", i - 1, dc.realFinished, dc.deadline);
        if(dc.realFinished > dc.deadline){
            avgDelay += dc.realFinished - dc.deadline;
            counter++;
        }
    }
    avgDelay /= counter;
    double var = 0;
    for (int i = 1; i < sz; i++) {
        deadlineC dc = deadArray[i];
        if(dc.realFinished > dc.deadline)
            var += pow((dc.realFinished - dc.deadline) - avgDelay, 2);
    }
    var /= counter;
    var = sqrtl(var);
    if(counter == 0){
        var = 0;
        avgDelay = 0;
    }
    double percentage = (double)counter;
    percentage /= sz - 1;
    percentage = 1 - percentage;
    percentage *= 100;
    printf("%%|| Processos que acabaram dentro da deadline = %.2lf%%\n",percentage);
    printf("Média de atraso = %lf\n",avgDelay);
    printf("Desvio padrão de atraso = %lf \n",var);
    printf("Mudanças de contexto = %d\n", get_ctx_changes());
    printf("NUMERO DE CORES = %ld\n||%%", numCPU);

    free(deadArray);
    // --------------------------------------------------------------------------------------------

}


void startProcess(int nCPU){
    if(pthread_create(&coreCPU[nCPU], NULL, processRoutine, NULL)) {
        fprintf(stderr, "Error creating thread! - CPU %d\n", nCPU);
        exit(1);
    }
}
int getCore(pthread_t id){
    int i;
    for(i = 0; i < numCPU && id != coreCPU[i]; i++);
    return i;
}

void *processRoutine(void *pinf){
    int dumbVar = 0; // just to consume CPU...
    int core;
    Process *p;

    pthread_mutex_lock(&mutex);
        core = getCore(pthread_self());
        p = pool[core];
        debugger(RUN_EVENT, p, core + 1);
        pool[core] = NULL;
    pthread_mutex_unlock(&mutex);

    Timer tnow = new_Timer();
    while(tnow->passed(tnow) < p->dt){
        dumbVar++;
    }
    destroy_Timer(tnow);

    pthread_mutex_lock(&mutex);
        debugger(EXIT_EVENT, p, getCore(pthread_self()) + 1);
        core = getCore(pthread_self());
        CPU[core] = IDLE_STATE;
        idleCPU++;
        ++outLine;
        outInfo[p->nLine - 1] = new_output(p->name, timer->passed(timer), timer->passed(timer) - p->t0, outLine);
        deadArray[p->nLine] = (deadlineC){timer->passed(timer), p->deadline};
        debugger(END_EVENT, p, outLine);
        pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    // Release the resources used by the thread to prevent memory leaks
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}



int cmpSJFThreaded(Process a, Process b){
    if (a.dt < b.dt)
        return -1;
    else if (a.dt > b.dt)
        return 1;
    return 0;
}
int cmpOutput(const void *a, const void *b){
    if(!a)
        return -1;
    if(!b)
        return 1;
    OutputData *o1 = *(OutputData **)a;
    OutputData *o2 =  *(OutputData **)b;
    if(o1->outN > o2->outN)
        return 1;
    if(o1->outN < o2->outN)
        return -1;
    return 0;
}

void writeOutput(int sz){
    qsort(outInfo, sz, sizeof(OutputData*), &cmpOutput);
    for (int i = 0; i < sz; i++) {
        OutputData *out = outInfo[i];
        write_outfile("%s %lf %lf\n",out->name, out->tf, out->treal);
        free(out->name);
        free(out);
    }
    free(outInfo);
}
