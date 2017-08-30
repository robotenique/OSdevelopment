/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 11/09/17
 *
 * Process scheduler simulator!
*/
#include "minPQ.h"
#include "error.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



int comparator(Process, Process);

int main(int argc, char const *argv[]) {
    // create a minPQ
    MinPQ pq = new_MinPQ(&comparator);

    for(int i = 0; i <= 1220; i++){
        // test process
        Process ptest;
        char temp[20] = "";
        sprintf(temp, "Processo t%02i", i);
        ptest.id = i;
        ptest.t0 = 5*i;
        ptest.dt = -0.0045*(i*i) + 9*i; // parabola >>SUAVE<<
        ptest.deadline = 45;
        ptest.name = estrdup(temp);
        // insert into pq
        pq->insert(pq, ptest);
    }
    debugPQ(pq);
    printf("DELETANDO ALGUNS PROCESSOS DA FILA DE PRIORIDADE....\n");
    for(int i = 0; i <= 1200; i++)
        pq->delMin(pq);
    debugPQ(pq);
    destroy_MinPQ(pq);

    return 0;
}

int comparator(Process a, Process b){
    return a.dt - b.dt;
}
