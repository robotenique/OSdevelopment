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


typedef struct ppack_t {
    Queue q;
    Stack *s;
} PPack;

void wakeup_next(PPack*);

double gtime = 0.0;
int finished = 0;
pthread_mutex_t gmtx;

Queue new_queue() {
    Queue q = (Queue)emalloc(sizeof(Node));
    q->p = NULL;
    q->next = q;
    q->prev = q;
    return q;
}

Node *queue_first(Queue q) {
    if (q->prev == q)
        return NULL;
    return q->prev;
}

void queue_add(Queue q, Node *n) {
    n->prev = q;
    n->next = q->next;
    q->next->prev = n;
    q->next = n;
    if (!queue_first(q))
        q->prev = n;
}

int queue_remove(Queue q) {
    if (!queue_first(q))
        return 0;
    Node *tmp = q->prev;
    q->prev = tmp->prev;
    q->prev->next = q;
    //free(tmp);
    return 1;
}

void queue_readd(Queue q) {
    Node *first = queue_first(q);
    queue_remove(q);
    queue_add(q, first);
}

Stack *new_stack(int size) {
    Stack *s = emalloc(sizeof(Stack));
    s->v = emalloc(size*sizeof(Node));
    s->i = size;
    return s;
}

Node *stack_remove(Stack *s) {
    if (s->i == -1)
        return NULL;
    (s->i)--;
    return &(s->v[s->i]);
}

Node *stack_top(Stack *s) {
    if (s->i == 0)
        return NULL;
    return &(s->v[s->i - 1]);
}

int wait(double ms) {
    struct timespec req, rem;

    req.tv_sec = floor(ms);
    req.tv_nsec = fmod(ms, 1) * 1000000000L;

    return nanosleep(&req , &rem);
}

void *run(void *arg) {
    Node *node = (Node *)arg;
    double w;

    while ((w = fmin(node->p->dt, 1.0))) {
        pthread_mutex_lock(&(node->mtx));
        wait(w);
        printf("=\n");
        node->p->dt -= w;
        gtime += w;
        printf("%s ran for %gs (%p)\n", node->p->name, w, node->p);
        //wakeup_next(pack);
        pthread_mutex_unlock(&gmtx);
    }

    printf("%s finished!\n", node->p->name);
    finished++;

    return NULL;
}

void queue_debug(Queue q) {
    printf("%p ", q->p);
    for (Node *n = q->next; n->p != NULL; n = n->next)
        printf("%s ", n->p->name);
    printf("%p\n", NULL);
}

void print_stack(Stack *s) {
    for (int i = 0; i < s->i; i++)
        printf("%s ", s->v[i].p->name);
    printf("\n");
}

void wakeup_next(PPack *pack) {
    Node *n = stack_top(pack->s);
    queue_debug(pack->q);
    //printf("%s will enter at queue with %g <= %g\n", n->p->name, n->p->t0, gtime);
    while (n && n->p->t0 <= gtime) {
        print_stack(pack->s);
        queue_add(pack->q, n);
        stack_remove(pack->s);
        pthread_create(&(n->t), NULL, &run, (void *)n);
        n = stack_top(pack->s);
        printf("%p\n", n);
    }
    queue_debug(pack->q);
    n = queue_first(pack->q);
    if (n->p->dt)
        queue_readd(pack->q);
    else
        queue_remove(pack->q);
    queue_debug(pack->q);
    n = queue_first(pack->q);
    printf("Unlocked %s\n", n->p->name);
    printf("gtime = %g\n", gtime);
    pthread_mutex_unlock(&(n->mtx));
}

void schedulerRoundRobin(ProcArray readyJobs, char *outfile) {
    PPack *pack;
    Stack *s = new_stack(readyJobs->i);
    pthread_mutex_init(&gmtx, NULL);

    for (int i = readyJobs->i - 1; i >= 0; i--)
        s->v[readyJobs->i - i - 1].p = &(readyJobs->v[i]);

    pack = emalloc(sizeof(PPack));
    pack->q = new_queue();
    pack->s = s;
    Node *tmp = stack_top(s);
    queue_add(pack->q, tmp);

    for (int i = 0; i < readyJobs->i; i++) {
        pthread_mutex_init(&(s->v[i].mtx), NULL);
        pthread_mutex_lock(&(s->v[i].mtx));
    }

    stack_remove(s);
    pthread_create(&(tmp->t), NULL, &run, (void *)tmp);
    pthread_mutex_lock(&gmtx);

    print_stack(s);

    while (finished < readyJobs->i) {
        if (queue_first(pack->q))
            wakeup_next(pack);
        else
            break;
        pthread_mutex_lock(&gmtx);
        // TODO: Add something to wait when there is no process running
    }

    // TODO: Change fake gtime to real time
    // TODO: Refactor the code

    pthread_exit(NULL);
}
