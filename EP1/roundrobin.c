/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 11/09/17
 *
 * Round Robin scheduler multithread implementation.
 */

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "roundrobin.h"
#include "process.h"
#include "error.h"
#include "utilities.h"
#include "deque.h"
#include "stack.h"

#define QUANTUM_VAL 1.0

static pthread_t **ranThreads;
static int finished = 0;
static pthread_mutex_t gmtx;
static pthread_mutex_t mtx;
static pthread_cond_t gcond;
static Timer timer;
static int numCPU;
static Core *cores;

static bool* firstTime;

static void *run(void *arg);



/*
 * Function: schedulerRoundRobin
 * --------------------------------------------------------
 * Simulates a Round Robin scheduler
 *
 * @args readyJobs : List of processes received
 *
 * @return
 */
void schedulerRoundRobin(ProcArray readyJobs) {
    int sz = readyJobs->i + 1;
    ranThreads = emalloc(sizeof(pthread_t*)*sz);
    firstTime = emalloc(sizeof(bool)*sz);
    Stack *pool = new_stack(readyJobs->i);
    Queue waitingP = new_queue();
    numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    Node *tmp;
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
    for (int i = readyJobs->i - 1; i >= 0; i--)
        pool->v[readyJobs->i - i - 1].p = &(readyJobs->v[i]);


    // Initiate the global mutex
    pthread_mutex_init(&gmtx, NULL);
    pthread_mutex_lock(&gmtx);
    pthread_cond_init(&gcond, NULL);
    pthread_mutex_init(&mtx, NULL);

    // Initiate all stack's mutexes
    for (int i = 0; i < readyJobs->i; i++) {
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
            pthread_join(*ranThreads[i], NULL);
    free(ranThreads);
    free(waitingP);
    free(pool->v);
    free(pool);
    free(cores);
    free(firstTime);
    destroy_Timer(timer);
    write_outfile("%d\n", get_ctx_changes());
}

/*
 * Function: runMT
 * --------------------------------------------------------
 * Thread function that simulates a process running
 *
 * @args arg : The process node
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
        w = fmin(n->p->dt, QUANTUM_VAL);

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
