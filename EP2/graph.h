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
    u_int* v; // Array
    u_int top; // Position to add the next element
};
typedef struct stack_s* Stack;

struct list_s{
  u_int to; // Where the edge points to
  struct list_s* next; // Pointer to next node
};

typedef struct list_s* List;

struct adj_s {
  List *vertexList; // List of the vertices
  u_int size; // Number of vertices
};

typedef struct adj_s* AdjList;

struct graph_s {
  u_int V; // Number of vertices
  AdjList adj; // the adjacency matrix
};

typedef struct graph_s * Grafinho;

typedef struct snode_s{
  Stack scc; // A stack with the nodes of the scc
  struct snode_s *next; // Pointer to next scc
} scc_node;

struct stacklist_s{
  scc_node *head; // Head of scclist
  u_int threshold; // the threshold
};

typedef struct stacklist_s* Stacklist;

/*---------------------------------------------------------------------*
 |                           Graph functions                           |
 *---------------------------------------------------------------------*/

/*
 * Function: new_grafinho
 * --------------------------------------------------------
 * Creates a new graph with the desired number of vertices, and
 * returns it
 *
 * @args num_vertex : total number of vertices
 *
 * @return the graph
 */
Grafinho new_grafinho(u_int num_vertex);

/*
 * Function: add_edge
 * --------------------------------------------------------
 * Add an edge from->to in the graph g
 *
 * @args g : the graph
 *       from : the node where the edge comes from
 *       to: the nde where the edge goes to
 *
 * @return
 */
void add_edge(Grafinho g, u_int from, u_int to);

/*
 * Function: SCC
 * --------------------------------------------------------
 * Find all the Strongly connected components in the graph g,
 * using the Tarjan Algorithm (recursive version), and add them
 * into the stacklist sl
 *
 * @args g : the graph
 *       sl : the initialized stack list (with the threshold set)
 *
 * @return
 */
void SCC(Grafinho g, Stacklist sl);

/*
 * Function: reset_grafinho
 * --------------------------------------------------------
 * Reset all edges of the graph g
 *
 * @args g : the graph
 *
 * @return
 */
void reset_grafinho(Grafinho g);

/*
 * Function: destroy_grafinho
 * --------------------------------------------------------
 * Destroys the graph, freeing its memory
 *
 * @args g : the graph
 *
 * @return
 */
void destroy_grafinho(Grafinho g);

/*
 * Function: new_Stacklist
 * --------------------------------------------------------
 * Creates a new stacklist with the given threshold, and
 * returns it
 *
 * @args threshold : the minimum number of vertices a given SCC
 *                   must have to be added into the stacklist
 *
 * @return the stacklist
 */
Stacklist new_Stacklist(u_int threshold);

/*
 * Function: destroy_Stacklist
 * --------------------------------------------------------
 * Destroys the stacklist, freeing its memory
 *
 * @args sl : the stacklist
 *
 * @return
 */
void destroy_Stacklist(Stacklist sl);

/*---------------------------------------------------------------------*
 |                           Stack functions                           |
 *---------------------------------------------------------------------*/

/*
 * Function: new_stack
 * --------------------------------------------------------
 * Creates a new static stack with fixed size
 *
 * @args  size :  the size of the stack
 *
 * @return  the created stack
 */
Stack new_stack(u_int size);

/*
 * Function: push
 * --------------------------------------------------------
 * Push an element to the stack
 *
 * @args id : the integer value to push into the stack
 *
 * @return
 */
void push(Stack, u_int);
/*
 * Function: pop
 * --------------------------------------------------------
 * Returns the top element in the stack
 *
 *
 * @return  the top element in the stack
 */
u_int pop(Stack);
/*
 * Function: empty
 * --------------------------------------------------------
 * check if the stack is empty or not
 *
 *
 * @return  true if it's empty, false otherwise
 */
bool empty(Stack);

/*
 * Function: top
 * --------------------------------------------------------
 * Returns the element at the top of the stack
 *
 * @return  the element at the top of the stack
 */
u_int top(Stack);

/*
 * Function: reset
 * --------------------------------------------------------
 * Reset a stack
 *
 *
 * @return
 */
void reset(Stack);

/*
 * Function: destroy_stack
 * --------------------------------------------------------
 * Destroys the stack, freeing its memory
 *
 *
 * @return
 */
void destroy_stack(Stack);

#endif
