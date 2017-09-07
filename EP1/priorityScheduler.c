/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 11/09/17
 *
 * Priority scheduler multithread implementation.
 */

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "priorityScheduler.h"
#include "process.h"
#include "error.h"
#include "utilities.h"
#include "deque.h"
#include "stack.h"

#define QUANTUM_VAL 1.0


// TODO: DON'T LET THE SIMULATOR RUN PROCESSES WITH DT = 0....

static Timer timer;
static pthread_mutex_t gmtx;
static pthread_cond_t gcond;
static pthread_mutex_t mtx;
static double *priority;
static int finished = 0;
static double var = 0;
static double avg = 0;
static int count = 0;
static bool SIGMOID = false;
static pthread_t **ranThreads;
static Core *cores;
static int numCPU;

// TODO: REMOVE EVERY MENTION OF THIS BUGGY THING AFTER STATISTICS ARE GENERATED!
static bool* firstTime;
// deadline related
static deadlineC *deadArray;

static void removeFromStats(double);
static double applyLogSigmoid(double);
static double calcQuanta(double);
static void *run(void*);
static double calculatePriority(Process*);
static void addToStats(double);
static void removeFromStats(double);



void schedulerPriority(ProcArray pQueue){
    int sz = pQueue->i + 1;
    // TODO: choose between one model... But test each of them
    SIGMOID = sigval;
    printf("RODANDO COM SIGMOID? %d\n",SIGMOID);
    Stack *pool = new_stack(pQueue->i);
    Queue waitingP = new_queue();
    Node *tmp;
    priority = emalloc(sizeof(double)*sz);
    deadArray = emalloc(sizeof(deadlineC)*sz);
    ranThreads = emalloc(sizeof(pthread_t*)*sz);
    firstTime = emalloc(sizeof(bool)*sz);
    numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    int runningPro = 0;
    for(int i = 0; i < sz; firstTime[i] = true,  i++);

    cores = emalloc(numCPU*sizeof(Core));
    for (int i = 0; i < numCPU; i++) {
        cores[i].ready = true;
        cores[i].n = NULL;
    }

    timer = new_Timer();

    // Transfer processes to stack
    for (int i = pQueue->i - 1; i >= 0; i--)
        pool->v[pQueue->i - i - 1].p = &(pQueue->v[i]);

    pthread_mutex_init(&gmtx, NULL);
    pthread_mutex_lock(&gmtx);
    pthread_cond_init(&gcond, NULL);
    pthread_mutex_init(&mtx, NULL);

    for (int i = 0; i < pQueue->i; i++) {
        pthread_mutex_init(&(pool->v[i].mtx), NULL);
        pthread_mutex_lock(&(pool->v[i].mtx));
    }

    while ((tmp = stack_top(pool)) || runningPro || queue_first(waitingP)) {
        if (!queue_first(waitingP) && !runningPro) {
            // Wait in idle mode if queue is empty
            double wt = tmp->p->t0 - timer->passed(timer);
            sleepFor(wt);
        }

        while (tmp && tmp->p->t0 <= timer->passed(timer)) {
            // Add new processes to queue if global time > t0
            priority[tmp->p->nLine] = calculatePriority(tmp->p);
            addToStats(priority[tmp->p->nLine]);
            queue_add(waitingP, tmp);
            debugger(ARRIVAL_EVENT, tmp->p, 0);
            stack_remove(pool);
            ranThreads[tmp->p->nLine] = &(tmp->t);
            pthread_create(&(tmp->t), NULL, &run, (void *)tmp);
            tmp = stack_top(pool);
        }

        pthread_mutex_lock(&mtx);
        runningPro = 0;
        for (int i = 0; i < numCPU; i++) {
            if (cores[i].ready && cores[i].n) {
                if (cores[i].n->p->dt)
                    queue_add(waitingP, cores[i].n);
                else
                    removeFromStats(priority[cores[i].n->p->nLine]);
                cores[i].n = NULL;
            }
        }
        for (int i = 0; i < numCPU; i++) {
            if (cores[i].ready && (tmp = queue_first(waitingP))) {
                cores[i].n = tmp;
                cores[i].ready = false;
                tmp->CPU = i;
                debugger(CONTEXT_EVENT, NULL, 0);
                queue_remove(waitingP);
                pthread_mutex_unlock(&(tmp->mtx));
            }
            if (cores[i].n)
                runningPro++;
        }
        pthread_mutex_unlock(&mtx);

        if (runningPro)
            pthread_cond_wait(&gcond, &gmtx);
    }

    // Freeing all threads...
    for(int i = 1; i < sz; i++)
        if(ranThreads[i] != NULL)
            pthread_join(*ranThreads[i],NULL);
    free(ranThreads);
    free(waitingP);
    free(pool->v);
    free(pool);
    free(priority);
    free(cores);
    free(firstTime);
    destroy_Timer(timer);

    write_outfile("%d\n", get_ctx_changes());

    // Deadline statistics TODO: remove this from the final code!
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
    percentage /= sz - 1;
    percentage = 1 - percentage;
    percentage *= 100;
    avgWaittime /= sz -1;
    printf("%%|| Processos que acabaram dentro da deadline = %.2lf%%\n",percentage);
    printf("Média de atraso = %lf\n",avgDelay);
    printf("Desvio padrão de atraso = %lf \n",var);
    printf("Mudanças de contexto = %d\n", get_ctx_changes());
    printf("Tempo de espera médio = %lf||%%\n", avgWaittime);
    free(deadArray);
}

/*
 * Function: applyLogSigmoid
 * --------------------------------------------------------
 * Receives a priority, and apply a sigmoid of the priority, then
 * apply a negative log function to the result. This will give
 * a quantum multiplier, from 1 to 10, according to the
 * priority received.
 *
 * @args  priority :  the priority of the process
 *
 * @return  returned value
 */
static double applyLogSigmoid(double priority){
     double qMult = -33*log10(pow(1+exp(-priority/25.0),-1)); // (max Quantum Multiplier = 10)
     // Don't use something less than 1 quantum multiplier, to be fair with everyone
     qMult = qMult < 1 ? 1 : qMult;
     return qMult;
}

/*
 * Function: calcQuanta
 * --------------------------------------------------------
 * Calculate how much quanta should be given to a process
 * based on its priority and the average and variance of
 * all running processes priorities. It assumes that the
 * priorities follow a normal distribution (which I'm not
 * sure), and, even if they doesn't, its a good dinamic
 * measuring system. This gives from 1 to 10 quanta.
 * TODO: Remove this
 * One can also change the way it claculates it using the
 * global variable SIGMOID, so it'll use apllyLogSigmoid
 * for the calculation.
 *
 * @args priority : process priority
 *
 * @return how much quanta the process will have this turn
 */
static double calcQuanta(double priority) {
    if(SIGMOID)
        return applyLogSigmoid(priority);
    double L = (!var)? 0 : (priority - avg)/sqrt(var);
    double scale = 2.25*fmin(4.0, fabs(L));
    printf("L = %g / scale = %g\n", L, scale);
    return QUANTUM_VAL*(scale + 1);
}

/*
 * Function: run
 * --------------------------------------------------------
 * The function that each process run. The processes will run
 * this for the quanta calculated for the processes, base on
 * the priority previously calculated.
 * When it has slept for this calculated time, it will unlock
 * the thread, for other processes to run.
 *
 * @args arg : the node of the process
 *
 * @return
 */
static void *run(void *arg) {
    Node *n = (Node *)arg;
    double w;

    deadlineC deadarr;
    do {
        //int dumbVar = 0; // just to consume CPU...
        pthread_mutex_lock(&(n->mtx));

        debugger(RUN_EVENT, n->p, n->CPU + 1);
        if(firstTime[n->p->nLine]){
            // The first time this process has run, it will save the waitTime...
            firstTime[n->p->nLine] = false;
            deadarr.waitTime = timer->passed(timer) - n->p->t0;
        }
        w = fmin(n->p->dt, calcQuanta(priority[n->p->nLine]));
        printf("Avg = %g / SD = %g\n", avg, sqrt(var));
        printf("Priority = %g / Quanta = %g\n", priority[n->p->nLine], w);

        // LETS CONSUME A LITTLE MORE CPU...
        /*Timer tnow = new_Timer();
        while(tnow->passed(tnow) < w){
            dumbVar++;
        }
        destroy_Timer(tnow);*/
        sleepFor(w);

        n->p->dt -= w;

        pthread_mutex_lock(&mtx);
        cores[n->CPU].ready = true;
        debugger(EXIT_EVENT, n->p, n->CPU + 1);
        pthread_mutex_unlock(&mtx);

        pthread_cond_signal(&gcond);
    } while (n->p->dt);

    finished++;
    deadarr.realFinished = timer->passed(timer);
    deadarr.deadline = n->p->deadline;
    deadArray[n->p->nLine] = deadarr;
    debugger(END_EVENT, n->p, finished);
    write_outfile("%s %lf %lf\n", n->p->name, timer->passed(timer), timer->passed(timer) - n->p->t0);

    return NULL;
}

/*
 * Function: calculatePriority
 * --------------------------------------------------------
 * Calculate the priority of a given process. The function to
 * calculate the priority of the process depends on the t0
 * of the process, the dt of the processes, and on a new value
 * called 'punctuality', which is how much time more we have to
 * complete the process until the deadline, or the "tightness"
 * of this interval. As we choose this to be a very important factor,
 * the priority depends linearly and quadratically on this factor.

 * @args  p :  a process
 *
 * @return  a double representing the priority of the process. The
 *          lower this number, the more priority it will have to run.
 */
static double calculatePriority(Process *p){
    double priority = 5000;
    double t0 = p->t0;
    double dt = p->dt;
    double punc = p->deadline - p->dt;
    /* These constants were calculated using machine learning
     * with gradient descent on a base of values that we estipulated.
     * The graph of this function can be visualized in the documents.
    */
    double d = 0.207715732988;
    double c = 0.21137699282;
    double b = 2.06241892813;
    double a = 0.00213475762298;
    if(punc > 0)
        priority = a*pow(punc, 2) + b*punc + c*dt + d*t0;
    return priority;
}

/*
 * Function: addToStats
 * --------------------------------------------------------
 * Add the new priority to global average and variance of
 * running processes
 *
 * @args priority : new process priority
 *
 * @return
 */
static void addToStats(double priority) {
    var = (count*(var + pow(avg, 2)) + pow(priority, 2))/(count + 1);
    avg = (avg*count + priority)/(count + 1);
    var -= pow(avg, 2);
    if (isnan(var))
        var = 0;
    count++;
}

/*
 * Function: removeFromStats
 * --------------------------------------------------------
 * Remove the priority of a finished process from global
 * average and variance of running processes
 *
 * @args priority : finished process priority
 *
 * @return
 */
static void removeFromStats(double priority) {
    if (count == 1) {
        var = 0;
        avg = 0;
    }
    else {
        var = (count*(var + pow(avg, 2)) - pow(priority, 2))/(count - 1);
        avg = (avg*count - priority)/(count - 1);
        var -= pow(avg, 2);
    }
    count--;
}
