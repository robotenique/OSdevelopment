#include <stdlib.h>
#include "graph.h"
#include "error.h"
#include "bikeStructures.h"

void add_SCCstack(Stacklist st, Stack newscc);

// Global variable for the SCC
static u_int timeG = 0;

u_int min(u_int a, u_int b) {
  if (a <= b)
    return a;
  return b;
}

/*---------------------------------------------------------------------*
 |                           Graph functions                           |
 *---------------------------------------------------------------------*/

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

void reset_grafinho(Grafinho g){
    for (int i = 0; i < g->adj->size; i++) {
        List tmp;
        while (g->adj->vertexList[i] != NULL) {
            tmp = g->adj->vertexList[i];
            g->adj->vertexList[i] = g->adj->vertexList[i]->next;
            free(tmp);
         }
    }
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

void SCC_aux(Grafinho g, u_int u, u_int disc[], u_int low[],
             Stack st, bool stackMember[], Stacklist sl){
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
    bool* stackMember = emalloc(g->V*sizeof(bool));
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
        if(adj->vertexList[i] == NULL)
            printf("Nothing from vertex %d...\n", i);
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
    printf("Debug Stacklist {\n");
    for (scc_node* x = sl->head; x != NULL; x = x->next) {
        for (size_t i = 0; i < x->scc->top - 1; i++)
            printf("%d , ", x->scc->v[i]);
        printf("%d\n", x->scc->v[x->scc->top - 1]);
    }
    printf("}\n");
}

/*---------------------------------------------------------------------
 |                           Stack functions                           |
  ---------------------------------------------------------------------*/

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
