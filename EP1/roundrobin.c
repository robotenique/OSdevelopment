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

void wakeup_next(Queue, Stack*);

int finished = 0;
pthread_mutex_t gmtx;
Timer timer;

/*
 * Function: iWait
 * --------------------------------------------------------
 * Special wait function for idle thread
 *
 * @args
 *
 * @return
 */
void *iWait(void *t) {
    double *dt = (double *)t;
    sleepFor(*dt);
    //printf("Esperei por %gs\n", *dt);
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
void *run(void *arg) {
    Node *n = (Node *)arg;
    double w;

    while ((w = fmin(n->p->dt, 1.0))) {
        pthread_mutex_lock(&(n->mtx));
        debugger(RUN_EVENT, *(n->p), 0);
        sleepFor(w);
        n->p->dt -= w;
        debugger(EXIT_EVENT, *(n->p), 0);
        pthread_mutex_unlock(&gmtx);
    }

    finished++;
    debugger(END_EVENT, *(n->p), finished);
    write_outfile("%s %lf %lf\n", n->p->name, timer->passed(timer), timer->passed(timer) - n->p->t0);

    return NULL;
}

// TODO: Remove this debug =======================================
void queue_debug(Queue q) {
    printf("%p ", NULL);
    for (Node *n = q->next; n->p != NULL; n = n->next)
        printf("%s ", n->p->name);
    printf("%p\n", NULL);
}

void print_stack(Stack *s) {
    for (int i = 0; i < s->i; i++)
        printf("%s ", s->v[i].p->name);
    printf("\n");
}
//================================================================

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
void wakeup_next(Queue q, Stack *s) {
    Node *n = stack_top(s);
    Node *mem;

    while (n && n->p->t0 <= timer->passed(timer)) {
        // Add new processes to queue if global time > t0
        queue_add(q, n);
        debugger(ARRIVAL_EVENT, *(n->p), 0);
        stack_remove(s);
        pthread_create(&(n->t), NULL, &run, (void *)n);
        n = stack_top(s);
    }

    // Readd the process to queue or remove it from queue
    if ((mem = queue_first(q)) && mem->p->dt)
        queue_readd(q);
    else
        queue_remove(q);

    // Start/restart the next process
    if ((n = queue_first(q)))
        pthread_mutex_unlock(&(n->mtx));
    if (mem != n && mem)
        debugger(CONTEXT_EVENT, *(mem->p), 0);
}

/*
 * Function: schedulerRoundRobin
 * --------------------------------------------------------
 * Simulates a Round Robin scheduler
 *
 * @args readyJobs : List of processes received
 *       outfile : Name of the log file
 *
 * @return
 */
void schedulerRoundRobin(ProcArray readyJobs, char *outfile) {
    Stack *s = new_stack(readyJobs->i);
    Queue q = new_queue();
    pthread_t idleThread;
    Node *tmp;
    double *wt = (double*)emalloc(sizeof(double));

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
            //printf("Esperando processos chegarem...\n");
            pthread_create(&idleThread, NULL, &iWait, (void *)wt);
        }
        pthread_mutex_lock(&gmtx);
        wakeup_next(q, s);
    }

    free(q);
    free(s->v);
    free(s);
    free(wt);
    pthread_exit(NULL);
}
