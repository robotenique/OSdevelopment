/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 11/09/17
 *
 * Shortest Job First scheduler multithread implementation.
 */

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "sjf.h"
#include "error.h"
#include "process.h"
#include "minPQ.h"
#include "utilities.h"
#define IDLE_STATE 0
#define RUNNING_STATE 1

//TODO: remove USELESS printf all across the EP

// Type to store info to write in the output file
typedef struct output{
    char *name;
    double tf;
    double treal;
    int outN; // line of the file to write
} OutputData;


static OutputData **outInfo; // Array with the outputs to write
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Global mutex
static pthread_mutex_t schedMutex = PTHREAD_MUTEX_INITIALIZER; // Mutex of the scheduler
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER; // Condition to receive signals from CPU cores
static pthread_t *coreCPU; // Our CPU cores
static Process **pool; // pool of processes, where the CPU cores get the next process to run
static unsigned int *CPU; // Array with the states of the CPU cores (IDLE_STATE/RUNNING_STATE)
static unsigned int idleCPU; // Number of CPU cores in the IDLE_STATE
static long numCPU;
static Timer timer; // Global timer
static int outLine; // Variable with the number of the line to write in the output file
static deadlineC *deadArray;


int cmpSJFThreaded(Process, Process);
void startProcess(int);
void *processRoutine(void *);
void writeOutput(int);
OutputData *new_output(char*, double, double, int);

/*
 * Function: schedulerSJF
 * --------------------------------------------------------
 * Receives a queue of processes to run.
 * The algorithm uses a MinPQ to sort the processes who have arrived by
 * dt. Every process that have arrived is inserted into the MinPQ.
 * The algorithm work as follows:
 * If there's still processes to run:
 *      If there's a free CPU core to run:
 *          put the process from the MinPQ into the pool of that CPU core;
 *          start the thread of that core;
 *      If all CPU cores are busy, the scheduler thread waits for a signal
 *      when any CPU core finishes a process.
 * We have multiple cores (threads) running at the same time processing their
 * related process from the pool. Whenever a core finishes the process, it will
 * send a signal to the scheduler thread. If the scheduler was waiting for a core
 * to be free, it will then proceed the scheduling process: Get all processes
 * that have arrived, insert then into the minPQ, and proceed as described early.
 * If no process has arrived yet, the scheduler will sleep.
 *
 *
 * @args  pQueue :  the queue of processes to run
 *
 * @return
 */
void schedulerSJF(ProcArray pQueue){
    int sz = pQueue->i + 1; // size auxiliar variable
    numCPU = sysconf(_SC_NPROCESSORS_ONLN); // get the number of CPUs of the system
    outInfo = emalloc(sizeof(OutputData*)*pQueue->i);
    coreCPU = emalloc(sizeof(pthread_t)*numCPU);
    CPU = emalloc(sizeof(unsigned int)*numCPU);
    pool = emalloc(sizeof(Process *)*numCPU);
    deadArray = emalloc(sizeof(deadlineC)*sz);
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
    // This is the main cycle of the scheduler
    while(pQueue->nextP < pQueue->i || !pPQ->isEmpty(pPQ)){
        double tNow = timer->passed(timer);
        int i;
        // If there's no process, the scheduler sleeps...
        if(pPQ->isEmpty(pPQ)){
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
                    //debugger(CONTEXT_EVENT, NULL, 0);
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

    // Write output
    writeOutput(sz - 1);
    write_outfile("%d\n", get_ctx_changes());

    // free everything
    destroy_Timer(timer);
    destroy_MinPQ(pPQ);
    free(coreCPU);
    free(CPU);
    free(pool);

    // TODO: remove deadline statistics later
    int counter = 0;
    double avgDelay = 0;
    double avgWaittime = 0.0;
    printf("\n\n");
    for (int i = 1; i < sz; i++) {
        deadlineC dc = deadArray[i];
        printf("Processo da linha %d : tReal = %lf , deadline = %lf\n", i - 1, dc.realFinished, dc.deadline);
        avgWaittime += dc.waitTime;
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
    avgWaittime /= sz - 1;
    percentage /= sz - 1;
    percentage = 1 - percentage;
    percentage *= 100;
    printf("%%|| Processos que acabaram dentro da deadline = %.2lf%%\n",percentage);
    printf("Média de atraso = %lf\n",avgDelay);
    printf("Desvio padrão de atraso = %lf \n",var);
    printf("Mudanças de contexto = %d\n", get_ctx_changes());
    printf("Tempo de espera médio = %lf\n", avgWaittime);
    printf("NUMERO DE CORES = %ld\n||%%", numCPU);

    free(deadArray);
    // --------------------------------------------------------------------------------------------

}

/*
 * Function: startProcess
 * --------------------------------------------------------
 * Start a process (a thread) in the cpu core provided.
 *
 * @args  nCPU :  int number of the cpu core to run
 *
 * @return
 */
void startProcess(int nCPU){
    if(pthread_create(&coreCPU[nCPU], NULL, processRoutine, NULL))
        die("Error creating thread! - CPU %d\n", nCPU);
}

/*
 * Function: getCore
 * --------------------------------------------------------
 * Returns the core a given thread is running
 *
 * @args id : The identifier of the thread
 *
 * @return the integer representing the core
 */
int getCore(pthread_t id){
    int i;
    for(i = 0; i < numCPU && id != coreCPU[i]; i++);
    return i;
}

/*
 * Function: processRoutine
 * --------------------------------------------------------
 * Represents what a core does: Get the process from its pool,
 * make some CPU work for a given time (here just making sums),
 * and then notify the scheduler that the work is done.
 *
 * @args pinf : required void* argument by pthread, useless here
 *
 * @return
 */
void *processRoutine(void *pinf){
    int dumbVar = 0; // just to consume CPU...
    int core;
    Process *p;
    deadlineC deadarr;

    // Prepare a process
    pthread_mutex_lock(&mutex);
        core = getCore(pthread_self());
        p = pool[core];
        debugger(RUN_EVENT, p, core + 1);
        deadarr.waitTime = timer->passed(timer) - p->t0;
        pool[core] = NULL;
    pthread_mutex_unlock(&mutex);
    sleepFor(p->dt);
    /*
    // Run the process itself for a given time
    Timer tnow = new_Timer();
    while(tnow->passed(tnow) < p->dt){
        dumbVar++;
    }
    destroy_Timer(tnow);
    */

    // Remove the process from the core, and send signal to the scheduler
    pthread_mutex_lock(&mutex);
        debugger(EXIT_EVENT, p, getCore(pthread_self()) + 1);
        core = getCore(pthread_self());
        CPU[core] = IDLE_STATE;
        idleCPU++;
        ++outLine;
        deadarr.realFinished = timer->passed(timer);
        deadarr.deadline = p->deadline;
        outInfo[p->nLine - 1] = new_output(p->name, timer->passed(timer), timer->passed(timer) - p->t0, outLine);
        deadArray[p->nLine] = deadarr;
        debugger(END_EVENT, p, outLine);
        pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    // Release the resources used by the thread to prevent memory leaks
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}

/*
 * Function: cmpSJFThreaded
 * --------------------------------------------------------
 * Priority function to use in the MinPQ
 *
 * @args a : the first Process
 *       b : the second process
 *
 * @return 1 if a > b, -1 if b < a, 0 otherwise
 */
int cmpSJFThreaded(Process a, Process b){
    if (a.dt < b.dt)
        return -1;
    else if (a.dt > b.dt)
        return 1;
    return 0;
}

/*
 * Function: cmpOutput
 * --------------------------------------------------------
 * Compare function to order the output by the number of
 * the line, to write in the correct order
 *
 * @args a : the first OutputData
 *       b : the second OutputData
 *
 * @return 1 if a > b, -1 if b < a, 0 otherwise
 */
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

/*
 * Function: new_output
 * --------------------------------------------------------
 * Create a new OutputData. Used every time a process finishes
 *
 * @args name : The name of the process
 * @args tf : when the process finished
 * @args treal : tf - t0 of the process
 * @args outN : Number of the line to write the process
 *
 * @return  A pointer to an OutputData
 */
OutputData *new_output(char *name, double tf, double treal, int outN){
    OutputData *ret = emalloc(sizeof(OutputData));
    ret->name = estrdup(name);
    ret->tf = tf;
    ret->treal = treal;
    ret->outN = outN;
    return ret;
}

/*
 * Function: writeOutput
 * --------------------------------------------------------
 * write all the info from the outInfo storage into the outfile.
 *
 * @args sz : The size of the outInfo storage
 *
 * @return
 */
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
