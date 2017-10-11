#include <stdio.h>
#include "graph.h"
#include "error.h"
#define SZ_DEFAULT =  4

u_int min(u_int a, u_int b) {
  if (a <= b)
    return a;
  return b;
}
// Global variable for the SCC
static u_int timeG = 0;

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

AdjList new_adjacencyList(u_int num_vertex){
  AdjList ret = emalloc(sizeof(struct adj_s));
  ret->vertexList = emalloc(num_vertex*sizeof(List));
  for (u_int i = 0; i < num_vertex; i++)
    ret->vertexList[i] = NULL;
  ret->size = num_vertex;
  return ret;
}

Grafinho new_grafinho(u_int num_vertex){
  Grafinho g = emalloc(sizeof(struct graph_2));
  g->V = num_vertex;
  g->adj = new_adjacencyList(g->V);
  return g;
}

void add_edge(Grafinho g, u_int from, u_int to) {
  AdjList adj = g->adj;
  List temp = emalloc(sizeof(struct list_s));
  temp->next = NULL;
  temp->to = to;
  if(adj->vertexList[from] == NULL){
    adj->vertexList[from] = temp;
  }
  else{
    temp->next = adj->vertexList[from];
    adj->vertexList[from] = temp;
  }
}

void add_SCCstack(Stacklist st, Stack newscc);

void SCC_aux(Grafinho g, u_int u, u_int disc[], u_int low[], Stack st, bool stackMember[], Stacklist sl){
  disc[u] = low[u] = ++timeG;
  push(st, u);
  stackMember[u] = true;
  for (List t = g->adj->vertexList[u]; t != NULL; t = t->next) {
    u_int v = t->to;
    if(disc[v] == -1){
      SCC_aux(g, v, disc, low, st, stackMember, sl);
      low[u] = min(low[u], low[v]);
    }
    else if(stackMember[v])
      low[u] = min(low[u], disc[v]);
  }
  u_int w = 0;
  if(low[u] == disc[u]) {
      Stack newscc = new_stack(g->V);
    while (top(st) != u) {
      w = top(st);
      push(newscc, w);
      printf("%d ", w);
      stackMember[w] = false;
      pop(st);
    }
    w = top(st);
    push(newscc, w);
    printf("%d\n", w);
    stackMember[w] = false;
    pop(st);
    add_SCCstack(sl, newscc);
  }
}

void SCC(Grafinho g, Stacklist sl) {
  u_int* disc = emalloc(g->V*sizeof(u_int));
  u_int* low = emalloc(g->V*sizeof(u_int));
  bool* stackMember = emalloc(g->V*sizeof(u_int));
  Stack st = new_stack(g->V);
  timeG = 0;
  for (size_t i = 0; i < g->V; i++) {
    disc[i] = -1;
    low[i] = -1;
    stackMember[i] = false;
  }
  for (size_t i = 0; i < g->V; i++) {
    if(disc[i] == -1)
      SCC_aux(g, i, disc, low, st, stackMember, sl);
  }


}

void debugAdj(AdjList adj) {
  for (u_int i = 0; i < adj->size; i++) {
    printf("FROM = %d :  ", i);
    for (List t = adj->vertexList[i]; t != NULL ; t = t->next)
      printf("%d, ", t->to);
    printf("\n");
  }
}

Stacklist new_Stacklist(u_int threshold){
  Stacklist sl = emalloc(sizeof(struct stacklist_s));
  sl->head = NULL;
  sl->threshold = threshold;
  return sl;
}

void add_SCCstack(Stacklist sl, Stack newscc){
    // Don't add if the SCC has more than the threshold number of components
    if(newscc->top < sl->threshold)
    return;
  scc_node* t = emalloc(sizeof(scc_node));
  t->scc = newscc;
  t->next = sl->head == NULL ? NULL : sl->head;
  sl->head = t;
}

void debugStacklist(Stacklist sl){
    for (scc_node* x = sl->head; x != NULL; x = x->next) {
        printf("Novo SCC: \n");
        while (!empty(x->scc))
            printf("%d, ", pop(x->scc));
        printf("\n");
    }
}

int main() {
    Grafinho g = new_grafinho(11);
    Stacklist mysccs = new_Stacklist(0);
    add_edge(g, 0, 1);add_edge(g, 0, 3);
    add_edge(g, 1, 2);add_edge(g, 1, 4);
    add_edge(g, 2, 0);add_edge(g, 2, 6);
    add_edge(g, 3, 2);
    add_edge(g, 4, 5);add_edge(g, 4, 6);
    add_edge(g, 5, 6);add_edge(g, 5, 7);add_edge(g, 5, 8);add_edge(g, 5, 9);
    add_edge(g, 6, 4);
    add_edge(g, 7, 9);
    add_edge(g, 8, 9);
    add_edge(g, 9, 8);
    //debugAdj(g->adj);
    SCC(g, mysccs);
    debugStacklist(mysccs);


    /*Graph g = new_graph(7);
    addEdge(g, 0, 1);
    addEdge(g, 0, 2);
    addEdge(g, 0, 3);
    addEdge(g, 1, 5);
    addEdge(g, 2, 4);
    addEdge(g, 3, 6);
    //addEdge(g, 2, 0);
    Stack cyc = getCycle(g);
    if (cyc == NULL)
    printf("NULL\n");
    else {
    for (u_int i = 0; i < cyc->top; i++)
    printf("%d\n", cyc->v[i]);
    }*/
    return 0;
}
