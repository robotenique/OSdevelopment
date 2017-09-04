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

#define QUANTUM_VAL 1.0

static Timer timer;
static pthread_mutex_t gmtx;
static double *quantum;
static int finished = 0;
static int deadlineCompleted = 0;
static double var = 0;
static double avg = 0;
static int count = 0;

static void *iWait(void *t) {
    double *dt = (double *)t;
    sleepFor(*dt);
    printf("Esperei por %gs\n", *dt);
    pthread_mutex_unlock(&gmtx);
    return NULL;
}

double calcQuanta(double priority) {
    double L = (priority - avg)/sqrt(var);
    double scale = 2.5*fmin(4.0, fabs(L));
    return QUANTUM_VAL*(11.0 - scale);
}

void *runPScheduler(void *arg) {
    Node *n = (Node *)arg;
    double w;

    while ((w = fmin(n->p->dt, calcQuanta(quantum[n->p->nLine])))) {
        pthread_mutex_lock(&(n->mtx));
        debugger(RUN_EVENT, *(n->p), 0);
        //printf("Quanta = %g\n", w);
        sleepFor(w);
        n->p->dt -= w;
        debugger(EXIT_EVENT, *(n->p), 0);
        pthread_mutex_unlock(&gmtx);
    }

    finished++;
    deadlineCompleted += timer->passed(timer) <= n->p->deadline ? 1 : 0;
    debugger(END_EVENT, *(n->p), finished);
    write_outfile("%s %lf %lf\n", n->p->name, timer->passed(timer), timer->passed(timer) - n->p->t0);

    return NULL;
}

double calculatePriority(Process p){
    //double qMult = 0.5;
    double priority = 0;
    double t0 = p.t0;
    double dt = p.dt;
    double punc = p.deadline - p.dt;
    double d = 0.207715732988;
    double c = 0.21137699282;
    double b = 2.06241892813;
    double a = 0.00213475762298;
    if(punc > 0)
        priority = a*pow(punc, 2) + b*punc + c*dt + d*t0;
    // qqMult = -67*log10(pow(1+exp(priority/47.0),-1)); (max Quantum Multiplier = 20)
    //qMult = -33*log10(pow(1+exp(priority/47.0),-1)); // (max Quantum Multiplier = 10)
    //qMult = qMult < 0.5 ? 0.5 : qMult;
    return priority;
}

void addToStats(double priority) {
    var = (count*(var + pow(avg, 2)) + pow(priority, 2))/(count + 1);
    avg = (avg*count + priority)/(count + 1);
    var -= pow(avg, 2);
    count++;
}

void removeFromStats(double priority) {
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

static void wakeup_next(Queue q, Stack *s){
    Node *n = stack_top(s);
    Node *mem;
    while (n && n->p->t0 <= timer->passed(timer)) {
        // set priority of process in quantum array
        quantum[n->p->nLine] = calculatePriority(*(n->p));
        addToStats(quantum[n->p->nLine]);
        // Add new processes to queue if global time > t0
        queue_add(q, n);
        debugger(ARRIVAL_EVENT, *(n->p), 0);
        stack_remove(s);
        pthread_create(&(n->t), NULL, &runPScheduler, (void *)n);
        n = stack_top(s);
    }
    // Readd the process to queue or remove it from queue
    if ((mem = queue_first(q)) && mem->p->dt)
        queue_readd(q);
    else if (mem){
        queue_remove(q);
        removeFromStats(quantum[mem->p->nLine]);
    }

    // Start/restart the next process
    if ((n = queue_first(q)))
        pthread_mutex_unlock(&(n->mtx));
    if (mem != n && mem)
        debugger(CONTEXT_EVENT, *(mem->p), 0);
}
void schedulerPriority(ProcArray pQueue, char *outfile){
    Stack *pool = new_stack(pQueue->i);
    Queue runningP = new_queue();
    pthread_t idleThread;
    timer = new_Timer();
    Node *tmp;
    quantum = emalloc(sizeof(unsigned int)*pQueue->i);
    // Transfer processes to stack
    for (int i = pQueue->i - 1; i >= 0; i--)
        pool->v[pQueue->i - i - 1].p = &(pQueue->v[i]);
    pthread_mutex_init(&gmtx, NULL);
    pthread_mutex_lock(&gmtx);
    for (int i = 0; i < pQueue->i; i++) {
        pthread_mutex_init(&(pool->v[i].mtx), NULL);
        pthread_mutex_lock(&(pool->v[i].mtx));
    }
    wakeup_next(runningP, pool);
    while (finished < pQueue->i) {
        if (!queue_first(runningP)) {
            if (!(tmp = stack_top(pool)))
                break;
            // Wait in idle mode if queue is empty
            double wt = tmp->p->t0 - timer->passed(timer);
            //printf("Esperando processos chegarem...\n");
            pthread_create(&idleThread, NULL, &iWait, &wt);
        }
        pthread_mutex_lock(&gmtx);
        wakeup_next(runningP, pool);
    }
    printf("Deadline completed = %lf%%\n", 100.0*(double)deadlineCompleted/(double)pQueue->i);
}
