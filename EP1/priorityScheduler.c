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


static Timer timer;
static pthread_mutex_t gmtx;
static pthread_cond_t gcond;
static pthread_mutex_t mtx;
static double *priority;
static int finished = 0;
static double var = 0;
static double avg = 0;
static int count = 0;
static pthread_t **ranThreads;
static Core *cores;
static int numCPU;
static double QUANTUM_VAL;

static bool* firstTime;

static void removeFromStats(double);
static double calcQuanta(double);
static void *run(void*);
static double calculatePriority(Process*);
static void addToStats(double);
static void removeFromStats(double);



void schedulerPriority(ProcArray pQueue){
    int sz = pQueue->i + 1;
    Stack *pool = new_stack(pQueue->i);
    Queue waitingP = new_queue();
    Node *tmp;
    priority = emalloc(sizeof(double)*sz);
    ranThreads = emalloc(sizeof(pthread_t*)*sz);
    firstTime = emalloc(sizeof(bool)*sz);
    numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    QUANTUM_VAL = 0.17857142857142858*numCPU + 0.2857142857142857;
    int runningPro = 0;
    for(int i = 0; i < sz; firstTime[i] = true,  i++);

    cores = emalloc(numCPU*sizeof(Core));
    for (int i = 0; i < numCPU; i++) {
        cores[i].ready = true;
        cores[i].n = NULL;
    }

    // Initiate global timer
    timer = new_Timer();

    // Transfer processes to stack
    for (int i = pQueue->i - 1; i >= 0; i--)
        pool->v[pQueue->i - i - 1].p = &(pQueue->v[i]);

    // Initiate the global mutex
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
            if (cores[i].n && cores[i].ready) {
                // Remove ready processes from cores and put them at the queue
                if (cores[i].n->p->dt)
                    queue_add(waitingP, cores[i].n);
                else
                    removeFromStats(priority[cores[i].n->p->nLine]);
            }
            if (cores[i].ready && (tmp = queue_first(waitingP))) {
                // Wake up processes from queue
                if (!(firstTime[tmp->p->nLine]))
                    debugger(CONTEXT_EVENT, NULL, 0);
                cores[i].n = tmp;
                cores[i].ready = false;
                tmp->CPU = i;
                queue_remove(waitingP);
                pthread_mutex_unlock(&(tmp->mtx));
            }
            if (!(cores[i].ready))
                runningPro++;
            else
                cores[i].n = NULL;
        }
        pthread_mutex_unlock(&mtx);

        if (runningPro)
            // Wait for some running process to finish
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
 *
 * @args priority : process priority
 *
 * @return how much quanta the process will have this turn
 */
static double calcQuanta(double priority) {
    double L = (!var)? 0 : (priority - avg)/sqrt(var);
    double scale = 2.25*fmin(4.0, fabs(L));
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

    do {
        //int dumbVar = 0; // just to consume CPU...
        pthread_mutex_lock(&(n->mtx));

        debugger(RUN_EVENT, n->p, n->CPU + 1);
        if(firstTime[n->p->nLine]){
            // The first time this process has run, it will save the waitTime...
            firstTime[n->p->nLine] = false;
        }
        w = fmin(n->p->dt, calcQuanta(priority[n->p->nLine]));
        /*
        // LETS CONSUME A LITTLE MORE CPU...
        Timer tnow = new_Timer();
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
