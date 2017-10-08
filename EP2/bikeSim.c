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
 #include "biker.h"
 #include "error.h"
 #include "randomizer.h"
 #include "debugger.h"

 void init(u_int num_bikers, u_int num_laps, u_int road_sz);
 void destroy(u_int num_bikers);

int main(int argc, char const *argv[]) {
    set_prog_name("bikeSim");
    /*if(argc < 5)
        die("Wrong number of arguments!\nUsage ./bikeSim <d> <n> <v> <debug>");
    u_int road_sz = atoi(argv[1]);
    u_int num_bikers = atoi(argv[2]);
    u_int num_laps = atoi(argv[3]);
    if (argc == 4)
        DEBUG_MODE = true;
    else
        DEBUG_MODE = false;*/
    u_int num_bikers = 20;
    u_int num_laps = 3;
    u_int road_sz = 3;
    DEBUG_MODE = true;
    init(num_bikers, num_laps, road_sz);
    debug_road();
    pthread_barrier_wait(&start_shot);
    u_int par = 0;

    while (sb->act_num_bikers != 0) {
        //printf("****MAIN***** ACTIVE BIKERS = %u\n", sb->act_num_bikers);
        //printf("****MAIN***** ESPERANDO BARR1\n");
        pthread_barrier_wait(&barr);
        if (par%3 == 0)
            debug_road();
        par++;
        if(sb->act_num_bikers == 0)
            break;
        //printf("****MAIN***** ESPERANDO BARR2\n");
        pthread_barrier_wait(&debugger_barr);
    }

    for (size_t i = 0; i < dummy_threads->i; i++) {
        printf("VRAU\n");
        pthread_join(dummy_threads->dummyT[i], NULL);
        printf("JOINED - %lu\n", i);
    }

    destroy(num_bikers);
    return 0;
}

/*
 * Function: init
 * --------------------------------------------------------
 * Initialization routine: Creates all the required global data
 * structures, such as the scoreboard, the barriers, speedway, etc
 * @args num_bikers : the total number of bikers
 *       num_laps : the total number of laps
 *       road_sz : the size (meters) of the road
 *
 * @return
 */
void init(u_int num_bikers, u_int num_laps, u_int road_sz) {
    if (DEBUG_MODE)
        print_prog_name();

    // The main barrier
    pthread_barrier_init(&barr, NULL, num_bikers + 1);
    // Debug barrier: wait the debugger, then proceed TODO: remove?
    pthread_barrier_init(&debugger_barr, NULL, num_bikers + 1);
    // Make all bikers start at the same time
    pthread_barrier_init(&start_shot, NULL, num_bikers + 1);

    create_speedway(road_sz, num_laps);
    broken = new_buffer(-1, num_bikers);
    sb = new_scoreboard(num_laps, num_bikers);
    create_dummy_threads(num_bikers);
    new_bikers(num_bikers);
    if (event(0.1))
        bikers[randint(0, num_bikers)]->fast = true;
}

/*
 * Function: destroy
 * --------------------------------------------------------
 * Destructor Function
 *
 * @args num_bikers : the total number of bikers
 *
 * @return
 */
void destroy(u_int num_bikers) {
    destroy_speedway();
    destroy_scoreboard(sb);
    destroy_bikers(num_bikers);
    destroy_buffer(broken);
}
