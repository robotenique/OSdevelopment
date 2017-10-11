#include <stdio.h>
#include "graph.h"
#include "error.h"
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
