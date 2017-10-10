#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "macros.h"

struct stack_s {
    u_int* v;
    u_int top;
};
typedef struct stack_s* Stack;

struct graph_s {
    Stack *vtcs;
    u_int size;
};
typedef struct graph_s* Graph;

Graph new_graph(u_int);

void addEdge(Graph, u_int, u_int);

Stack getCycles(Graph);

void reset_graph(Graph);

void destroy_graph(Graph);

Stack new_stack();

void push(Stack, u_int);

u_int pop(Stack);

bool empty(Stack);

u_int top(Stack);

void reset(Stack);

void destroy_stack(Stack);

#endif
