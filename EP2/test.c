#include <stdio.h>
#include "graph.h"

int main() {
    Graph g = new_graph(7);
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
        for (int i = 0; i < cyc->top; i++)
            printf("%d\n", cyc->v[i]);
    }
    return 0;
}
