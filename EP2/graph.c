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

Stack getCycles(Graph g) {
    Stack s = new_stack(g->size);
    Stack l = new_stack(g->size);
    Stack path = new_stack(speedway.length+1);
    Stack res = new_stack(g->size);
    bool *marked;
    bool brk = false;
    u_int aux;
    marked = emalloc(g->size*sizeof(bool));
    for (size_t i = 0; i < g->size; i++)
        marked[i] = false;
    for (size_t i = 0; i < g->size; i++) {
        if (marked[i]) continue;
        push(s, i);
        push(l, -1);
        while (!empty(s)) {
            brk = false;
            while (!brk && !empty(s)) {
                u_int last = pop(l);
                u_int now = pop(s);
                printf("pop %u <- %u\n", now, last);
                marked[now] = true;
                while ((aux = top(path)) != last && aux != -1 && last != -1)
                printf("pop %u from path\n", pop(path));
                push(path, now);
                printf("push %u at path\n", now);
                Stack edges = g->vtcs[now];
                for (u_int i = 0; i < edges->top; i++) {
                    printf("%d == %d\n", path->v[0], edges->v[i]);
                    if (!empty(path) && path->v[0] == edges->v[i]) {
                        push(path, edges->v[i]);
                        printf("BREAK\n");
                        for (int i = 0; i < path->top; i++)
                        printf("%d\n", path->v[i]);
                        brk = true;
                    }
                    if (!marked[edges->v[i]]) {
                        push(s, edges->v[i]);
                        push(l, now);
                        printf("push %u <- %u\n", edges->v[i], now);
                    }
                }
            }
            if (path->top > 1 && path->v[0] == top(path)) {
                for (int i = 0; i < path->top; i++)
                    printf("%d\n", path->v[i]);
                pop(path);
                for (int i = 0; i < path->top; i++)
                    push(res, path->v[i]);
            }
        }
        reset(path);
    }
    return res;
}

void reset_graph(Graph g) {
    for (int i = 0; i < g->size; i++)
        reset(g->vtcs[i]);
}

void destroy_graph(Graph g) {
    for (int i = 0; i < g->size; i++)
        destroy_stack(g->vtcs[i]);
    free(g->vtcs);
    free(g);
}

Stack new_stack(u_int size) {
    Stack s = emalloc(sizeof(struct stack_s));
    s->top = 0;
    s->v = emalloc(size*sizeof(u_int));
    return s;
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

void reset(Stack s) {
    s->top = 0;
}

void destroy_stack(Stack s) {
    free(s->v);
    free(s);
}
