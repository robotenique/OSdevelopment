#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "process.h"
#include "error.h"
#include "utilities.h"
#include "deque.h"
#include "stack.h"
#define CPU_CORE 1

static void wakeup_next(Queue, Stack*);
static pthread_t **ranThreads;
static int finished = 0;
static pthread_mutex_t gmtx;
static Timer timer;

static bool* firstTime;

/*
 * Function: iWait
 * --------------------------------------------------------
 * Special wait function for idle thread
 *
 * @args
 *
 * @return
 */
static void *iWait(void *t) {
    double *dt = (double *)t;
    sleepFor(*dt);
    pthread_mutex_unlock(&gmtx);
    return NULL;
}

/*
 * Function: run
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
        pthread_mutex_lock(&(n->mtx));
        debugger(RUN_EVENT, n->p, CPU_CORE);
        if(firstTime[n->p->nLine]){
            // The first time this process has run, it will save the waitTime...
            firstTime[n->p->nLine] = false;
        }
        w = fmin(n->p->dt, 1.0);
        sleepFor(w);
        n->p->dt -= w;
        debugger(EXIT_EVENT, n->p, CPU_CORE);
        pthread_mutex_unlock(&gmtx);
    } while (n->p->dt);

    finished++;
    debugger(END_EVENT, n->p, finished);
    write_outfile("%s %lf %lf\n", n->p->name, timer->passed(timer), timer->passed(timer) - n->p->t0);

    return NULL;
}

/*
 * Function: wakeup_next
 * --------------------------------------------------------
 * Manages the process queue, adding and removing it's
 * processes and waking up the next process
 *
 * @args q : Process queue
 *       s : Process stack
 *
 * @return
 */
static void wakeup_next(Queue q, Stack *s) {
    Node *n = stack_top(s);
    Node *mem = NULL;
    Node *notEmpty = queue_first(q);

    while (n && n->p->t0 <= timer->passed(timer)) {
        // Add new processes to queue if global time > t0
        queue_add(q, n);
        debugger(ARRIVAL_EVENT, n->p, 0);
        stack_remove(s);
        ranThreads[n->p->nLine] = &(n->t);
        pthread_create(&(n->t), NULL, &run, (void *)n);
        n = stack_top(s);
    }

    // Readd the process to queue or remove it from queue
    if (notEmpty) {
        if ((mem = queue_first(q)) && mem->p->dt)
            queue_readd(q);
        else
            queue_remove(q);
    }

    // Start/restart the next process
    if ((n = queue_first(q))) {
        pthread_mutex_unlock(&(n->mtx));
    }
    if (mem != n && n)
        debugger(CONTEXT_EVENT, NULL, 0);
}

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
    for(int i = 0; i < sz; firstTime[i] = true,  i++);
    Stack *s = new_stack(readyJobs->i);
    Queue q = new_queue();
    pthread_t idleThread;
    Node *tmp;
    double *wt = (double*)emalloc(sizeof(double));
    bool notIdle = true;

    // Initiate timer
    timer = new_Timer();

    // Transfer processes to stack
    for (int i = readyJobs->i - 1; i >= 0; i--)
        s->v[readyJobs->i - i - 1].p = &(readyJobs->v[i]);

    // Initiate the global mutex
    pthread_mutex_init(&gmtx, NULL);
    pthread_mutex_lock(&gmtx);

    // Initiate all stack's mutexes
    for (int i = 0; i < readyJobs->i; i++) {
        pthread_mutex_init(&(s->v[i].mtx), NULL);
        pthread_mutex_lock(&(s->v[i].mtx));
    }

    // Wake up the first process of the queue (if there is one)
    wakeup_next(q, s);

    while (finished < readyJobs->i) {
        if (!queue_first(q)) {
            if (!(tmp = stack_top(s)))
                break;
            // Wait in idle mode if queue is empty
            *wt = tmp->p->t0 - timer->passed(timer);
            ranThreads[0] = &idleThread;
            notIdle = false;
            pthread_create(&idleThread, NULL, &iWait, (void *)wt);
        }
        pthread_mutex_lock(&gmtx);
        wakeup_next(q, s);
    }

    // Freeing all threads...
    for(int i = notIdle; i < sz; i++)
        if(ranThreads[i] != NULL)
            pthread_join(*ranThreads[i],NULL);
    free(ranThreads);
    free(q);
    free(s->v);
    free(s);
    free(wt);
    free(firstTime);
    destroy_Timer(timer);
    write_outfile("%d\n", get_ctx_changes());

}
