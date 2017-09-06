#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
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
static PNode *post;
static int numCPU;

// TODO: REMOVE EVERY MENTION OF THIS BUGGY THING AFTER STATISTICS ARE GENERATED!
static bool* firstTime;
// deadline related
static deadlineC *deadArray;





static double applyLogSigmoid(double priority){
    //double qMult = -67*log10(pow(1+exp(-priority/47.0),-1)); // (max Quantum Multiplier = 20)
    double qMult = -33*log10(pow(1+exp(-priority/25.0),-1)); // (max Quantum Multiplier = 10)
    qMult = qMult < 1 ? 1 : qMult;
    return qMult;
}

static double calcQuanta(double priority) {
    if(SIGMOID)
        return applyLogSigmoid(priority);
    double L = (!var)? 0 : (priority - avg)/sqrt(var);
    double scale = 2.25*fmin(4.0, fabs(L));
    printf("L = %g / scale = %g\n", L, scale);
    return QUANTUM_VAL*(scale + 1);
}

static void *run(void *arg) {
    Node *n = (Node *)arg;
    double w;

    deadlineC deadarr;
    do {
        int dumbVar = 0; // just to consume CPU...
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
        Timer tnow = new_Timer();
        while(tnow->passed(tnow) < w){
            dumbVar++;
        }
        destroy_Timer(tnow);

        n->p->dt -= w;

        pthread_mutex_lock(&mtx);
        post[n->CPU].ready = true;
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

static double calculatePriority(Process *p){
    double priority = 5000;
    double t0 = p->t0;
    double dt = p->dt;
    double punc = p->deadline - p->dt;
    double d = 0.207715732988;
    double c = 0.21137699282;
    double b = 2.06241892813;
    double a = 0.00213475762298;
    if(punc > 0)
        priority = a*pow(punc, 2) + b*punc + c*dt + d*t0;
    return priority;
}

static void addToStats(double priority) {
    var = (count*(var + pow(avg, 2)) + pow(priority, 2))/(count + 1);
    avg = (avg*count + priority)/(count + 1);
    var -= pow(avg, 2);
    if (isnan(var))
        var = 0;
    count++;
}

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

void schedulerPriorityMT(ProcArray pQueue){
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

    post = emalloc(numCPU*sizeof(PNode));
    for (int i = 0; i < numCPU; i++) {
        post[i].ready = true;
        post[i].n = NULL;
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
            if (post[i].ready && post[i].n) {
                if (post[i].n->p->dt)
                    queue_add(waitingP, post[i].n);
                else
                    removeFromStats(priority[post[i].n->p->nLine]);
                post[i].n = NULL;
            }
        }
        for (int i = 0; i < numCPU; i++) {
            if (post[i].ready && (tmp = queue_first(waitingP))) {
                post[i].n = tmp;
                post[i].ready = false;
                tmp->CPU = i;
                debugger(CONTEXT_EVENT, NULL, 0);
                queue_remove(waitingP);
                pthread_mutex_unlock(&(tmp->mtx));
            }
            if (post[i].n)
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
    free(post);
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
