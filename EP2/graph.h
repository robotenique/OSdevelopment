#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "bikeStructures.h"

struct queue_s {
    u_int* v;
    u_int beg, end, size;
};
typedef struct queue_s* Queue;

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

Stack getCycle(Graph);

Queue new_queue();

void enqueue(Queue, u_int);

u_int dequeue(Queue);

Stack new_stack();

void push(Stack, u_int);

u_int pop(Stack);

bool empty(Stack);

u_int top(Stack);

#endif
