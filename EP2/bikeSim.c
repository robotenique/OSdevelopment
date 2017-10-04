/*
 * @author: João Gabriel Basi Nº USP: 9793801
 * @author: Juliano Garcia de Oliveira Nº USP: 9277086
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
    u_int numLaps = 10;
    DEBUG_MODE = true;
    if (DEBUG_MODE)
        print_prog_name();

    // The main barrier
    pthread_barrier_init(&barr, NULL, numBikers + 1);
    // Debug barrier: wait the debugger, then proceed TODO: remove?
    pthread_barrier_init(&debugger_barr, NULL, numBikers + 1);
    // Make all bikers start at the same time
    pthread_barrier_init(&start_shot, NULL, numBikers + 1);

    create_speedway(roadSz, numLaps);
    broken = new_buffer(-1, numBikers);
    dummies = emalloc(numBikers*sizeof(pthread_t));
    sb = new_scoreboard(numLaps, numBikers);
    new_bikers(numBikers);
    debug_road();
    pthread_barrier_wait(&start_shot);
    u_int par = 0;

    while (sb->act_num_bikers != 0) {
        pthread_barrier_wait(&barr);
        if (par%3 == 0)
            debug_road();
        par++;
        pthread_barrier_wait(&debugger_barr);
    }

    destroy_speedway();
    destroy_scoreboard(sb);
    destroy_bikers(numBikers);

    return 0;
}
