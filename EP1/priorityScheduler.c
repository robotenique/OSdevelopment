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
static double var = 0;
static double avg = 0;
static int count = 0;
static bool SIGMOID = false;
static pthread_t **ranThreads;
// deadline related
static deadlineC *deadArray;



static void *iWait(void *t) {
    double *dt = (double *)t;
    sleepFor(*dt);
    printf("Esperei por %gs\n", *dt);
    pthread_mutex_unlock(&gmtx);
    return NULL;
}

double applyLogSigmoid(double priority){
    double qMult = -67*log10(pow(1+exp(-priority/47.0),-1)); // (max Quantum Multiplier = 20)
    //qMult = -33*log10(pow(1+exp(-priority/47.0),-1)); // (max Quantum Multiplier = 10)
    qMult = qMult < 1 ? 1 : qMult;
    return qMult;
}

double calcQuanta(double priority) {
    if(SIGMOID)
        return applyLogSigmoid(priority);
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
    deadArray[n->p->nLine] = (deadlineC){timer->passed(timer), n->p->deadline};
    debugger(END_EVENT, *(n->p), finished);
    write_outfile("%s %lf %lf\n", n->p->name, timer->passed(timer), timer->passed(timer) - n->p->t0);

    return NULL;
}

double calculatePriority(Process p){
    double priority = 5000;
    double t0 = p.t0;
    double dt = p.dt;
    double punc = p.deadline - p.dt;
    double d = 0.207715732988;
    double c = 0.21137699282;
    double b = 2.06241892813;
    double a = 0.00213475762298;
    if(punc > 0)
        priority = a*pow(punc, 2) + b*punc + c*dt + d*t0;
    //qMult = -67*log10(pow(1+exp(-priority/47.0),-1)); // (max Quantum Multiplier = 20)
    //qMult = -33*log10(pow(1+exp(-priority/47.0),-1)); // (max Quantum Multiplier = 10)
    //qMult = qMult < 1 ? 1 : qMult;
    //printf("\nCalculado prioridade para %s (%lf , %lf, %lf) = %lf (qMult = %lf)\n",p.name, t0, dt, punc, priority, qMult);
    //return qMult;
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
        ranThreads[n->p->nLine] = &(n->t);
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

void schedulerPriority(ProcArray pQueue){
    int sz = pQueue->i + 1;
    // TODO: choose between one model... But test each of them
    SIGMOID = false;
    Stack *pool = new_stack(pQueue->i);
    Queue runningP = new_queue();
    pthread_t idleThread;
    Node *tmp;
    quantum = emalloc(sizeof(double)*sz);
    deadArray = emalloc(sizeof(deadlineC)*sz);
    ranThreads = emalloc(sizeof(pthread_t*)*sz);
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
            ranThreads[0] = &idleThread;
            pthread_create(&idleThread, NULL, &iWait, &wt);
        }
        pthread_mutex_lock(&gmtx);
        wakeup_next(runningP, pool);
    }
    // Freeing all threads...
    for(int i = 0; i < sz; i++)
        if(ranThreads[i] != NULL)
            pthread_join(*ranThreads[i],NULL);
    free(ranThreads);
    free(runningP);
    free(pool->v);
    free(pool);
    free(quantum);
    destroy_Timer(timer);

    write_outfile("%d\n", get_ctx_changes());

    // Deadline statistics TODO: remove this from the final code!
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
    printf("Desvio padrão de atraso = %lf ||%%\n",var);
    free(deadArray);
}
