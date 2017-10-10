#include <stdlib.h>
#include "graph.h"
#include "error.h"
#include "bikeStructures.h"

Graph new_graph(u_int numBikers) {
    Graph g = emalloc(sizeof(struct graph_s));
    g->vtcs = emalloc(numBikers*sizeof(Stack));
    for (size_t i = 0; i < numBikers; i++)
        g->vtcs[i] = new_stack(4);
    g->size = numBikers;
    return g;
}

void addEdge(Graph g, u_int from, u_int to) {
    push(g->vtcs[from], to);
}

Stack getCycle(Graph g) {
    Stack s = new_stack(g->size);
    Stack path = new_stack(speedway.length+1);
    Stack l = new_stack(g->size);
    bool *marked;
    bool brk = false;
    marked = emalloc(g->size*sizeof(bool));
    for (size_t i = 0; i < g->size; i++)
        marked[i] = false;
    for (size_t i = 0; i < g->size; i++) {
        if (marked[i]) continue;
        push(s, i);
        push(l, -1);
        while (!empty(s) && !brk) {
            u_int last = pop(l);
            u_int now = pop(s);
            printf("pop %u <- %u\n", now, last);
            marked[now] = true;
            while (top(path) != last && last != -1)
                printf("pop %u from path\n", pop(path));
            push(path, now);
            printf("push %u at path\n", now);
            Stack edges = g->vtcs[now];
            for (u_int i = 0; i < edges->top; i++) {
                if (!empty(path) && path->v[0] == edges->v[i]) {
                    push(path, edges->v[i]);
                    brk = true;
                }
                if (!marked[edges->v[i]]) {
                    push(s, edges->v[i]);
                    push(l, now);
                    printf("push %u <- %u\n", edges->v[i], now);
                }
            }
        }
        if (brk) break;
    }
    if (empty(path) || path->v[0] != top(path))
        return NULL;
    pop(path);
    return path;
}

Queue new_queue(u_int numBikers) {
    Queue q = emalloc(sizeof(struct queue_s));
    q->beg = 0;
    q->end = 0;
    q->v = emalloc(numBikers*sizeof(u_int));
    q->size = numBikers;
    return q;
}

void enqueue(Queue q, u_int id) {
    q->v[q->end] = id;
    q->end = (q->end + 1)%q->size;
}

u_int dequeue(Queue q) {
    u_int mem = q->v[q->beg];
    q->beg = (q->beg + 1)%q->size;
    return mem;
}

Stack new_stack(u_int size) {
    Stack s = emalloc(sizeof(struct stack_s));
    s->top = 0;
    s->v = emalloc(size*sizeof(u_int));
}

void push(Stack s, u_int id) {
    s->v[s->top] = id;
    s->top++;
}

bool empty(Stack s) {
    return (s->top == 0);
}

u_int pop(Stack s) {
    if (empty(s))
        return -1;
    s->top--;
    return s->v[s->top];
}

u_int top(Stack s) {
    if (empty(s))
        return -1;
    return s->v[s->top-1];
}
