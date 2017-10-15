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
 #include <unistd.h> // sleep
 #include <time.h> // nanosleep
 #include <math.h> // floor
 #include "bikeStructures.h"
 #include "biker.h"
 #include "error.h"
 #include "randomizer.h"
 #include "debugger.h"
 #include "graph.h"

 void init(u_int num_bikers, u_int num_laps, u_int road_sz);
 void destroy(u_int num_bikers);

int main(int argc, char const *argv[]) {
    set_prog_name("bikeSim");
    if(argc < 4)
        die("Wrong number of arguments!\nUsage ./bikeSim <d> <n> <v> <debug>");
    u_int road_sz = atoi(argv[1]);
    u_int num_bikers = atoi(argv[2]);
    u_int num_laps = atoi(argv[3]);
    /*if(road_sz <= 249)
        die("Error in road size (d)");
    else if(num_bikers <= 5 || num_bikers > 5*road_sz)
        die("Error in number of bikers (n)");
    else if(num_laps%20 != 0)
        die("Error in number of laps (v)");*/
    if (argc == 5)
        DEBUG_MODE = true;
    else
        DEBUG_MODE = false;

    init(num_bikers, num_laps, road_sz);
    if(DEBUG_MODE)
        debug_road_better();
    pthread_barrier_wait(&start_shot);
    u_int par = 1;

    pthread_barrier_wait(&barr);

    if(DEBUG_MODE)
        debug_road_better();
    while (sb.act_num_bikers != 0) {

        pthread_barrier_wait(&debugger_barr);

        if (sb.act_num_bikers >= speedway.length) {
            // Reset moveTypes array
            speedway.moveTypes[0] = DOWN;
            for (int i = 1; i < NUM_LANES-1; i++)
            speedway.moveTypes[i] = TOPDOWN;
            speedway.moveTypes[NUM_LANES-1] = TOP;

            // Get the SCCs
            SCC(speedway.g, speedway.sccl);

            // Put NONE at all lines that have a vertice that belongs to some SCC
            for (scc_node* x = speedway.sccl->head; x != NULL; x = x->next)
                for (int i = 0; i < x->scc->top; i++)
                    speedway.moveTypes[bikers[x->scc->v[i]]->j] = NONE;

            // Invert directions
            for (int i = 0; i < NUM_LANES - 1; i++) {
                if ((speedway.moveTypes[i] & DOWN) && !(speedway.moveTypes[i+1] & TOP)) {
                    speedway.moveTypes[i] ^= DOWN;
                    speedway.moveTypes[i+1] |= TOP;
                }
                else if (!(speedway.moveTypes[i] & DOWN) && (speedway.moveTypes[i+1] & TOP)) {
                    speedway.moveTypes[i] |= DOWN;
                    speedway.moveTypes[i+1] ^= TOP;
                }
            }

            // Assign move types to all SCC's vertices
            for (scc_node* x = speedway.sccl->head; x != NULL; x = x->next) {
                while (!empty(x->scc)) {
                    Biker b = bikers[pop(x->scc)];
                    b->moveType = speedway.moveTypes[b->j];
                }
            }

            // Reset the graph
            reset_grafinho(speedway.g);
        }

        pthread_barrier_wait(&prep_barr);
        pthread_barrier_wait(&barr);

        if (DEBUG_MODE && (par%3 == 0 || sb.foundFast) && sb.act_num_bikers != 0)
            debug_road_better();
        par++;
    }

    for (size_t i = 0; i < dummy_threads->i; i++)
        pthread_join(dummy_threads->dummyT[i], NULL);
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
    // Debug barrier: wait the debugger, then proceed
    pthread_barrier_init(&debugger_barr, NULL, num_bikers + 1);
    // Barrier that makes all bikers finish at the same time
    pthread_barrier_init(&prep_barr, NULL, num_bikers + 1);
    // Make all bikers start at the same time
    pthread_barrier_init(&start_shot, NULL, num_bikers + 1);

    create_speedway(road_sz, num_laps, num_bikers);
    broken = new_buffer(-1, num_bikers);
    new_scoreboard(num_laps, num_bikers);
    create_dummy_threads(num_bikers);
    new_bikers(num_bikers);
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
    destroy_scoreboard();
    destroy_bikers(num_bikers);
    destroy_buffer(broken);
    destroy_dummy_threads();
}
