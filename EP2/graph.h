/*
 * @author: João Gabriel Basi Nº USP: 9793801
 * @author: Juliano Garcia de Oliveira Nº USP: 9277086
 *
 * MAC0422
 * 16/10/17
 *
 * Header for graph and stack functions
 */
#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "macros.h"
/*---------------------------------------------------------------------*
 |                           Type definitions                          |
 *---------------------------------------------------------------------*/

struct stack_s {
    u_int* v;
    u_int top;
};
typedef struct stack_s* Stack;

struct list_s{
  u_int to;
  struct list_s* next;
};

typedef struct list_s* List;

struct adj_s {
  List *vertexList;
  u_int size;
};

typedef struct adj_s* AdjList;

struct graph_2 {
  u_int V;
  AdjList adj;
};

typedef struct graph_2 * Grafinho;

typedef struct snode_s{
  Stack scc;
  struct snode_s *next;
} scc_node;

struct stacklist_s{
  scc_node *head;
  u_int threshold;
};

typedef struct stacklist_s* Stacklist;

typedef struct graph_s* Graph;

/*---------------------------------------------------------------------*
 |                           Graph functions                           |
 *---------------------------------------------------------------------*/

Grafinho new_grafinho(u_int num_vertex);

void add_edge(Grafinho g, u_int from, u_int to);

void SCC(Grafinho g, Stacklist sl);

void reset_grafinho(Grafinho g);

void destroy_grafinho(Grafinho g);

Stacklist new_Stacklist(u_int threshold);

void destroy_Stacklist(Stacklist sl);

// TODO: Remove this two below
void debugAdj(AdjList adj);

void debugStacklist(Stacklist sl);

/*---------------------------------------------------------------------*
 |                           Stack functions                           |
 *---------------------------------------------------------------------*/

Stack new_stack();

void push(Stack, u_int);

u_int pop(Stack);

bool empty(Stack);

u_int top(Stack);

void reset(Stack);

void destroy_stack(Stack);

#endif
