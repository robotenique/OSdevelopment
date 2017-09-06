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

typedef struct post_node_t {
    Node *n;
    bool ready;
} PNode;

static deadlineC *deadArray;
void wakeup_nextMT(Queue, Stack*);
static pthread_t **ranThreads;
static int finished = 0;
static pthread_mutex_t gmtx;
static pthread_mutex_t mtx;
static pthread_cond_t gcond;
static Timer timer;
static int numCPU;
static PNode **post;
static int *inverse;

// TODO: REMOVE EVERY MENTION OF THIS BUGGY THING AFTER STATISTICS ARE GENERATED!
static bool* firstTime;


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
    deadlineC deadarr;
    do {
        int core;
        int dumbVar = 0; // just to consume CPU...

        pthread_mutex_lock(&(n->mtx));

        pthread_mutex_lock(&mtx);
        core = inverse[n->p->nLine - 1] + 1; // TODO: is this correct?
        debugger(RUN_EVENT, n->p, core); // TODO: display the correct number of the CPU
        if(firstTime[n->p->nLine]){
            // The first time this process has run, it will save the waitTime...
            firstTime[n->p->nLine] = false;
            deadarr.waitTime = timer->passed(timer) - n->p->t0;
        }
        w = fmin(n->p->dt, QUANTUM_VAL);
        pthread_mutex_unlock(&mtx);

        Timer tnow = new_Timer();
        while(tnow->passed(tnow) < w){
            dumbVar++;
        }
        destroy_Timer(tnow);

        // sleepFor(w); LETS CONSUME A LITTLE MORE CPU...
        n->p->dt -= w;

        //printf("%s acabou\n", n->p->name);
        //printf("HEY%s\n", n->p->name);

        pthread_mutex_lock(&mtx);
        post[inverse[n->p->nLine - 1]]->ready = true;
        debugger(EXIT_EVENT, n->p, core);
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
    firstTime = emalloc(sizeof(bool)*sz);
    for(int i = 0; i < sz; firstTime[i] = true,  i++);
    Stack *s = new_stack(readyJobs->i);
    Queue q = new_queue();
    numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    Node *tmp;
    int runningPro = 0;

    post = emalloc(numCPU*sizeof(PNode*));
    for (int i = 0; i < numCPU; i++) {
        post[i] = emalloc(sizeof(PNode));
        post[i]->ready = true;
        post[i]->n = NULL;
    }
    inverse = emalloc(readyJobs->i*sizeof(int));

    // Initiate global timer
    timer = new_Timer();

    // Transfer processes to stack
    for (int i = readyJobs->i - 1; i >= 0; i--) {
        inverse[i] = -1;
        s->v[readyJobs->i - i - 1].p = &(readyJobs->v[i]);
    }

    // Initiate the global mutex
    pthread_mutex_init(&gmtx, NULL);
    pthread_mutex_lock(&gmtx);
    pthread_cond_init(&gcond, NULL);
    pthread_mutex_init(&mtx, NULL);

    // Initiate all stack's mutexes
    for (int i = 0; i < readyJobs->i; i++) {
        pthread_mutex_init(&(s->v[i].mtx), NULL);
        pthread_mutex_lock(&(s->v[i].mtx));
    }

    while ((tmp = stack_top(s)) || runningPro || queue_first(q)) {
        if (!queue_first(q) && !runningPro) {
            // Wait in idle mode if queue is empty
            double wt = tmp->p->t0 - timer->passed(timer);
            sleepFor(wt);
        }

        while (tmp && tmp->p->t0 <= timer->passed(timer)) {
            // Add new processes to queue if global time > t0
            queue_add(q, tmp);
            debugger(ARRIVAL_EVENT, tmp->p, 0);
            stack_remove(s);
            ranThreads[tmp->p->nLine] = &(tmp->t);
            pthread_create(&(tmp->t), NULL, &runMT, (void *)tmp);
            tmp = stack_top(s);
        }

        pthread_mutex_lock(&mtx);
        runningPro = 0;
        for (int i = 0; i < numCPU; i++) {
            if (post[i]->ready && post[i]->n) {
                if (post[i]->n->p->dt)
                    queue_add(q, post[i]->n);
                post[i]->n = NULL;
            }
        }
        for (int i = 0; i < numCPU; i++) {
            if (post[i]->ready && (tmp = queue_first(q))) {
                post[i]->n = tmp;
                post[i]->ready = false;
                inverse[tmp->p->nLine - 1] = i; // TODO :  quer dizer que o processo vai rodar no core i + 1?
                debugger(CONTEXT_EVENT, NULL, 0);
                queue_remove(q);
                pthread_mutex_unlock(&(tmp->mtx));
            }
            if (post[i]->n)
                runningPro++;
        }
        pthread_mutex_unlock(&mtx);

        if (runningPro)
            pthread_cond_wait(&gcond, &gmtx);
    }

    // Freeing all threads...
    for(int i = 1; i < sz; i++)
        if(ranThreads[i] != NULL)
            pthread_join(*ranThreads[i], NULL);
    free(ranThreads);
    free(q);
    free(s->v);
    free(s);
    free(inverse);
    for (int i = 0; i < numCPU; i++)
        free(post[i]);
    free(post);
    destroy_Timer(timer);
    write_outfile("%d\n", get_ctx_changes());

    // TODO: remove deadline statistics later
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
    // --------------------------------------------------------------------------------------------

}
