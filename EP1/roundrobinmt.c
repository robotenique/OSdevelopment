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
#define IDLE_STATE 0
#define RUNNING_STATE 1

static deadlineC *deadArray;
void wakeup_nextMT(Queue, Stack*);
static pthread_t **ranThreads;
static int finished = 0;
static pthread_mutex_t gmtx;
static pthread_mutex_t pmtx;
static pthread_mutex_t mtx;
static pthread_cond_t gcond;
static int idleCPU;
static Timer timer;
static unsigned int *CPU;
static int numCPU;
static int post = 0;

/*
 * Function: runMT
 * --------------------------------------------------------
 * Thread function that simulates a process running
 *
 * @args arg : The process node
 *
 * @return
 */
void *runMT(void *arg) {
    Node *n = (Node *)arg;
    double w;

    do {
        pthread_mutex_lock(&(n->mtx));

        pthread_mutex_lock(&mtx);
        debugger(RUN_EVENT, n->p, 0);
        idleCPU--;
        w = fmin(n->p->dt, 1.0);
        pthread_mutex_unlock(&mtx);

        sleepFor(w);
        n->p->dt -= w;

        pthread_mutex_lock(&pmtx);
        printf("HEY%s\n", n->p->name);
        pthread_mutex_lock(&mtx);
        idleCPU++;
        post = n->p->nLine;
        debugger(EXIT_EVENT, n->p, 0);
        pthread_mutex_unlock(&mtx);

        pthread_cond_signal(&gcond);
    } while (n->p->dt);

    finished++;
    deadArray[n->p->nLine] = (deadlineC){timer->passed(timer), n->p->deadline};
    debugger(END_EVENT, n->p, finished);
    write_outfile("%s %lf %lf\n", n->p->name, timer->passed(timer), timer->passed(timer) - n->p->t0);

    return NULL;
}

// TODO: Remove this debug =======================================
void print_stack2(Stack *s) {
    for (int i = 0; i < s->i; i++)
        printf("%s ", s->v[i].p->name);
    printf("\n");
}
//================================================================



/*
 * Function: wakeup_nextMT
 * --------------------------------------------------------
 * Manages the process queue, adding and removing it's
 * processes and waking up the next process
 *
 * @args q : Process queue
 *       s : Process stack
 *
 * @return
 */
void wakeup_nextMT(Queue q, Stack *s) {
    Node *n = stack_top(s);

    print_stack2(s);

    //printf("%p\n", timer);

    while (n && n->p->t0 <= timer->passed(timer)) {
        // Add new processes to queue if global time > t0
        queue_add(q, n);
        debugger(ARRIVAL_EVENT, n->p, 0);
        stack_remove(s);
        ranThreads[n->p->nLine] = &(n->t);
        pthread_create(&(n->t), NULL, &runMT, (void *)n);
        n = stack_top(s);
    }

    if (post) {
        n = &(s->v[s->size - post]);
        if (n->p->dt)
            queue_add(q, n);
        post = 0;
        if ((n = queue_first(q))) {
            queue_remove(q);
            idleCPU--;
            pthread_mutex_unlock(&(n->mtx));
            debugger(CONTEXT_EVENT, NULL, 0);
        }
        pthread_mutex_unlock(&pmtx);
    }
    else if ((n = queue_first(q)) && idleCPU) {
        pthread_mutex_lock(&pmtx);
        queue_remove(q);
        idleCPU--;
        pthread_mutex_unlock(&(n->mtx));
        debugger(CONTEXT_EVENT, NULL, 0);
        pthread_mutex_unlock(&pmtx);

    }
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
void schedulerRoundRobinMT(ProcArray readyJobs) {
    int sz = readyJobs->i + 1;
    ranThreads = emalloc(sizeof(pthread_t*)*sz);
    deadArray = emalloc(sizeof(deadlineC)*sz);
    Stack *s = new_stack(readyJobs->i);
    Queue q = new_queue();
    numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    idleCPU = numCPU;
    CPU = emalloc(sizeof(unsigned int)*numCPU);
    Node *tmp;
    //bool notIdle = true;

    // Initiate timer
    timer = new_Timer();

    // Transfer processes to stack
    for (int i = readyJobs->i - 1; i >= 0; i--)
        s->v[readyJobs->i - i - 1].p = &(readyJobs->v[i]);

    // Initiate the global mutex
    pthread_mutex_init(&gmtx, NULL);
    pthread_mutex_lock(&gmtx);
    pthread_cond_init(&gcond, NULL);
    pthread_mutex_init(&pmtx, NULL);
    pthread_mutex_init(&mtx, NULL);

    // Initiate all stack's mutexes
    for (int i = 0; i < readyJobs->i; i++) {
        pthread_mutex_init(&(s->v[i].mtx), NULL);
        pthread_mutex_lock(&(s->v[i].mtx));
    }

    // Wake up the first process of the queue (if there is one)
    //wakeup_nextMT(q, s);

    while ((tmp = stack_top(s))) {
        if (!queue_first(q) && idleCPU == numCPU) {
            // Wait in idle mode if queue is empty
            double wt = tmp->p->t0 - timer->passed(timer);
            sleepFor(wt);
        }
        print_stack2(s);

        while (tmp && tmp->p->t0 <= timer->passed(timer)) {
            // Add new processes to queue if global time > t0
            queue_add(q, tmp);
            debugger(ARRIVAL_EVENT, tmp->p, 0);
            stack_remove(s);
            ranThreads[tmp->p->nLine] = &(tmp->t);
            pthread_create(&(tmp->t), NULL, &runMT, (void *)tmp);
            tmp = stack_top(s);
        }

        //printf("HEY4\n");

        pthread_mutex_lock(&mtx);
        if (post) {
            tmp = &(s->v[s->size - post]);
            if (tmp->p->dt)
            queue_add(q, tmp);
            post = 0;
            //printf("HEY3\n");
            pthread_mutex_unlock(&pmtx);
        }

        //printf("HEY5\n");

        while ((tmp = queue_first(q)) && idleCPU) {
            //printf("HEY2\n");
            queue_remove(q);
            pthread_mutex_unlock(&(tmp->mtx));
            debugger(CONTEXT_EVENT, NULL, 0);
        }
        pthread_mutex_unlock(&mtx);

        //queue_debug(q);

        if (!idleCPU || !queue_first(q)) {
            printf("HEY\n");
            pthread_cond_wait(&gcond, &gmtx);
        }
    }

    // Freeing all threads...
    for(int i = 1; i < sz; i++)
        if(ranThreads[i] != NULL)
            pthread_join(*ranThreads[i], NULL);
    free(ranThreads);
    free(q);
    free(s->v);
    free(s);
    free(CPU);
    destroy_Timer(timer);
    write_outfile("%d\n", get_ctx_changes());

    // TODO: remove deadline statistics later
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
    printf("Desvio padrão de atraso = %lf \n",var);
    printf("Mudanças de contexto = %d\n||%%", get_ctx_changes());

    free(deadArray);
    // --------------------------------------------------------------------------------------------

}
