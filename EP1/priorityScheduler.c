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
// Deadline related
typedef struct deadlineC{
    double realFinished;
    double deadline;
}deadlineC;
static deadlineC *deadArray;



static void *iWait(void *t) {
    double *dt = (double *)t;
    sleepFor(*dt);
    printf("Esperei por %gs\n", *dt);
    pthread_mutex_unlock(&gmtx);
    return NULL;
}

void *runPScheduler(void *arg) {
    Node *n = (Node *)arg;
    double w;

    while ((w = fmin(n->p->dt, QUANTUM_VAL*quantum[n->p->nLine]))) {
        pthread_mutex_lock(&(n->mtx));
        debugger(RUN_EVENT, *(n->p), 0);
        sleepFor(w);
        n->p->dt -= w;
        debugger(EXIT_EVENT, *(n->p), 0);
        pthread_mutex_unlock(&gmtx);
    }

    finished++;
    deadArray[n->p->nLine] = (deadlineC){timer->passed(timer), n->p->deadline};
    debugger(END_EVENT, *(n->p), finished);
    write_outfile("%s %lf %lf\n", n->p->name, timer->passed(timer), timer->passed(timer) - n->p->t0);

    return NULL;
}

double calculatePriority(Process p){
    double qMult = 0.5;
    double priority = 4700;
    double t0 = p.t0;
    double dt = p.dt;
    double punc = p.deadline - p.dt;
    double d = 0.207715732988;
    double c = 0.21137699282;
    double b = 2.06241892813;
    double a = 0.00213475762298;
    if(punc > 0)
        priority = a*pow(punc, 2) + b*punc + c*dt + d*t0;
    qMult = -67*log10(pow(1+exp(-priority/47.0),-1)); // (max Quantum Multiplier = 20)
    //qMult = -33*log10(pow(1+exp(-priority/47.0),-1)); // (max Quantum Multiplier = 10)
    qMult = qMult < 1 ? 1 : qMult;
    //printf("\nCalculado prioridade para %s (%lf , %lf, %lf) = %lf (qMult = %lf)\n",p.name, t0, dt, punc, priority, qMult);
    return qMult;
}

static void wakeup_next(Queue q, Stack *s){
    Node *n = stack_top(s);
    Node *mem;
    while (n && n->p->t0 <= timer->passed(timer)) {
        // set priority of process in quantum array
        quantum[n->p->nLine] = calculatePriority(*(n->p));
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
    else
        queue_remove(q);

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
    Node *tmp;
    quantum = emalloc(sizeof(double)*pQueue->i);
    deadArray = emalloc(sizeof(deadlineC)*pQueue->i);
    timer = new_Timer();
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
    // Deadline statistics TODO: remove this from the final code!
    int counter = 0;
    double avgDelay = 0;
    for (int i = 1; i < pQueue->i + 1; i++) {
        deadlineC dc = deadArray[i];
        printf("Processo da linha %d : tReal = %lf , deadline = %lf\n", i - 1, dc.realFinished, dc.deadline);
        if(dc.realFinished > dc.deadline){
            avgDelay += dc.realFinished - dc.deadline;
            counter++;
        }
    }
    avgDelay /= counter;
    double var = 0;
    for (int i = 1; i < pQueue->i + 1; i++) {
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
    printf("Processos que acabaram dentro da deadline = %lf%%\n",100.0*(1-(double)counter/(double)pQueue->i));
    printf("Média de atraso = %lf\n",avgDelay);
    printf("Desvio padrão de atraso = %lf\n",var);
}
