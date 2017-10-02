/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 16/10/17
 *
 * Main file of the bike sprint simulator
 */
 #include <stdio.h>
 #include <pthread.h>
 #include "bikeStructures.h"
 #include "error.h"
 #include "randomizer.h"
 #include "debugger.h"

int main(int argc, char const *argv[]) {
    set_prog_name("bikeSim");
    /*if(argc < 5)
        die("Wrong number of arguments!\nUsage ./bikeSim <d> <n> <v> <debug>");
    u_int roadSz = atoi(argv[1]);
    u_int numBikers = atoi(argv[2]);
    u_int numLaps = atoi(argv[3]);
    if (argc == 4)
        DEBUG_MODE = true;
    else
        DEBUG_MODE = false;*/
    u_int roadSz = 80;
    u_int numBikers = 20;
    u_int numLaps = 50;
    DEBUG_MODE = true;
    if (DEBUG_MODE) {
        print_prog_name();
    }


    pthread_barrier_init(&barr, NULL, numBikers+1);
    pthread_barrier_init(&barr2, NULL, numBikers+1);

    create_speedway(roadSz);
    sb = new_scoreboard(20, numBikers);
    bikers = emalloc(numBikers*sizeof(Biker));
    new_bikers(numBikers);
    printf("bikers criados\n");
    debug_road();
    destroy_speedway();
    destroy_scoreboard(sb);
    exit(0);

    // Sometimes some bikers are deadlocked and the race can't proceed
    // due to the barrier
    for (int i = 0; i < 20; i++) {
        pthread_barrier_wait(&barr);
        debug_road();
        pthread_barrier_wait(&barr2);

    }
    destroy_speedway();
    for (int i = 0; i < numBikers; i++) {
        free(bikers[i]->thread);
        free(bikers[i]->mtxs);
    }
    destroy_scoreboard(sb);

    return 0;
}
