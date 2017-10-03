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
    u_int numLaps = 50;
    DEBUG_MODE = true;
    if (DEBUG_MODE)
        print_prog_name();

    pthread_barrier_init(&barr, NULL, numBikers+1);
    pthread_barrier_init(&barr2, NULL, numBikers+1);
    pthread_barrier_init(&beg_shot, NULL, numBikers+1);

    create_speedway(roadSz, numLaps);
    sb = new_scoreboard(numLaps, numBikers);
    bikers = emalloc(numBikers*sizeof(Biker));
    new_bikers(numBikers);
    printf("bikers criados\n");
    debug_road();
    pthread_barrier_wait(&beg_shot);
    u_int par = 0;

    for (int i = 0; i < 20; i++) {
        pthread_barrier_wait(&barr);
        if (par%3 == 0)
            debug_road();
        par++;
        pthread_barrier_wait(&barr2);
    }

    destroy_speedway();
    destroy_scoreboard(sb);
    destroy_bikers(numBikers);

    return 0;
}
