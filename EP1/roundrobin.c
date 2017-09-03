#include "process.h"
#include "error.h"
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct node_t {
    Process *p;
    pthread_mutex_t mtx;
    pthread_t t;
    struct node_t *next, *prev;
} Node;

typedef Node* Queue;

typedef struct stack_t {
    Node *v;
    int i;
} Stack;

void wakeup_next(Queue, Stack*);

double gtime = 0.0;
int finished = 0;
pthread_mutex_t gmtx;

/*
 * Function: new_queue
 * --------------------------------------------------------
 * Creates a new Queue
 *
 * @args None
 *
 * @return  The new queue
 */
Queue new_queue() {
    Queue q = (Queue)emalloc(sizeof(Node));
    q->p = NULL;
    q->next = q;
    q->prev = q;
    return q;
}

/*
 * Function: queue_first
 * --------------------------------------------------------
 * Returns the first Node of the Queue (the last of the list)
 *
 * @args  q :  Queue
 *
 * @return  A pointer to the first Node
 */
Node *queue_first(Queue q) {
    if (q->prev == q)
        return NULL;
    return q->prev;
}

/*
 * Function: queue_add
 * --------------------------------------------------------
 * Enqueue a new Node
 *
 * @args n : A pointer to the new Node
 *
 * @return
 */
void queue_add(Queue q, Node *n) {
    n->prev = q;
    n->next = q->next;
    q->next->prev = n;
    q->next = n;
    if (!queue_first(q))
        q->prev = n;
}

/*
 * Function: queue_remove
 * --------------------------------------------------------
 * Removes the first Node of the Queue (the last of the list)
 * and returns 0 if the Queue is empty and 1 otherwise
 *
 * @args q : Queue
 *
 * @return 0 if the Queue is empty and 1 otherwise
 */
int queue_remove(Queue q) {
    if (!queue_first(q))
        return 0;
    Node *tmp = q->prev;
    q->prev = tmp->prev;
    q->prev->next = q;
    return 1;
}

/*
 * Function: queue_readd
 * --------------------------------------------------------
 * Readds the first Node of the Queue to it
 *
 * @args q : Queue
 *
 * @return
 */
void queue_readd(Queue q) {
    Node *first = queue_first(q);
    queue_remove(q);
    queue_add(q, first);
}

/*
 * Function: new_stack
 * --------------------------------------------------------
 * Creates a new Node Stack
 *
 * @args size : The size of the Stack
 *
 * @return A pointer to the Stack
 */
Stack *new_stack(int size) {
    Stack *s = emalloc(sizeof(Stack));
    s->v = emalloc(size*sizeof(Node));
    s->i = size;
    return s;
}

/*
 * Function: stack_remove
 * --------------------------------------------------------
 * Pops the top Node of the Stack
 *
 * @args s : Stack*
 *
 * @return A pointer to the top Node
 */
Node *stack_remove(Stack *s) {
    if (s->i == 0)
        return NULL;
    (s->i)--;
    return &(s->v[s->i]);
}

/*
 * Function: stack_top
 * --------------------------------------------------------
 * Returns the top Node of the Stack
 *
 * @args s : Stack*
 *
 * @return A pointer to the top Node
 */
Node *stack_top(Stack *s) {
    if (s->i == 0)
        return NULL;
    return &(s->v[s->i - 1]);
}

int wait(double t) {
    struct timespec req, rem;

    req.tv_sec = floor(t);
    req.tv_nsec = fmod(t, 1) * 1e9;

    return nanosleep(&req , &rem);
}

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
    wait(*dt);
    gtime += *dt;
    printf("Esperei por %gs\n", *dt);
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
        printf("==> %s entrou na CPU!\n", n->p->name);
        wait(w);
        n->p->dt -= w;
        gtime += w;
        printf("<== %s saiu da CPU!\n", n->p->name);
        pthread_mutex_unlock(&gmtx);
    }

    printf("** %s (linha trace = %d) finalizou no sistema **\n", n->p->name, n->p->nLine);
    finished++;

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
    while (n && n->p->t0 <= gtime) {
        queue_add(q, n);
        printf("** %s (linha trace = %d) chegou no sistema **\n", n->p->name, n->p->nLine);
        stack_remove(s);
        pthread_create(&(n->t), NULL, &run, (void *)n);
        n = stack_top(s);
    }
    if ((n = queue_first(q)) && n->p->dt)
        queue_readd(q);
    else
        queue_remove(q);
    if ((n = queue_first(q)))
        pthread_mutex_unlock(&(n->mtx));
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

    pthread_mutex_init(&gmtx, NULL);
    pthread_mutex_lock(&gmtx);

    for (int i = readyJobs->i - 1; i >= 0; i--)
        s->v[readyJobs->i - i - 1].p = &(readyJobs->v[i]);

    for (int i = 0; i < readyJobs->i; i++) {
        pthread_mutex_init(&(s->v[i].mtx), NULL);
        pthread_mutex_lock(&(s->v[i].mtx));
    }

    wakeup_next(q, s);

    while (finished < readyJobs->i) {
        if (!queue_first(q)) {
            if (!(tmp = stack_top(s)))
                break;
            *wt = tmp->p->t0 - gtime;
            printf("Esperando processos chegarem...\n");
            pthread_create(&idleThread, NULL, &iWait, (void *)wt);
        }
        pthread_mutex_lock(&gmtx);
        wakeup_next(q, s);
    }

    // TODO: Change fake gtime to real time

    free(q);
    free(s->v);
    free(s);
    free(wt);
    pthread_exit(NULL);
}
