/*
 * @author: João Gabriel Basi Nº USP: 9793801
 * @author: Juliano Garcia de Oliveira Nº USP: 9277086
 *
 * MAC0422
 * 16/10/17
 *
 * Header file for the bike structures
 */
#ifndef __BIKE_STRUCTURES_H__
#define __BIKE_STRUCTURES_H__
#include <stdio.h>
#include <pthread.h>
#include "macros.h"
#include "graph.h"
#include "biker.h"

/* Complex types definition (structs) */
struct dummy_s {
    pthread_mutex_t dummy_mtx;
    pthread_t *dummyT;
    u_int i;
    u_int size;
    void *(* dummy_func)(void *arg);
    void(*run_next)(struct dummy_s*);
};

typedef struct dummy_s* DummyThreads;

struct score_s {
    u_int id, score;
    u_lint time;
};

struct buffer_s {
    struct score_s *data;
    u_int lap, i, size;
    pthread_mutex_t mtx;
    void(*append)(struct buffer_s*, u_int, u_int, u_lint);
};

typedef struct buffer_s* Buffer;

typedef struct scbr_s {
    Buffer *scores;
    bool foundFast;
    u_int n, act_num_bikers;
    u_int *tot_num_bikers;
    pthread_mutex_t scbr_mtx;
    void(*add_score)(Biker);
} Scoreboard;

typedef struct road_s {
    pthread_mutex_t **mtxs; // Theirs (each road position)
    pthread_mutex_t mymtx; // Mine (the Road itself)
    u_int **road;
    u_int *nbpl; // Number of bikers per lane
    Move *moveTypes; // Move type of each lane
    Stacklist sccl;
    Grafinho g;
    u_int length, laps, num_bikers;
    bool (*exists)(int i, int j);
} Road;

/* Global Variables */
bool DEBUG_MODE;
DummyThreads dummy_threads;
Road speedway;
Scoreboard sb;
Buffer broken;
Biker *bikers;
pthread_barrier_t barr;
pthread_barrier_t debugger_barr;
pthread_barrier_t prep_barr;
pthread_barrier_t start_shot;

/*
 * Function: create_speedway
 * --------------------------------------------------------
 * Create the global speedway. The speedway is a global variable
 * which has the road matrix (d x 10 matrix), the length is d,
 * and the lanes are (by default) 10.
 *
 * @args d : u_int which represents the length of the speedway
 *
 * @return
 */
void create_speedway(u_int d, u_int laps, u_int num_bikers);

/*
 * Function: destroy_speedway
 * --------------------------------------------------------
 * Destroy the global speedway, freeing the memory
 *
 * @args
 *
 * @return
 */
void destroy_speedway();

/*
 * Function: new_scoreboard
 * --------------------------------------------------------
 * Create the global scoreboard
 *
 * @args laps : The quantity of laps
 *       num_bikers : The number of bikers
 *
 * @return
 */
void new_scoreboard(u_int laps, u_int num_bikers);

/*
 * Function: destroy_scoreboard
 * --------------------------------------------------------
 * Destroy the global scoreboard, freeing the memory
 *
 * @args
 *
 * @return
 */
void destroy_scoreboard();

/*
 * Function: new_buffer
 * --------------------------------------------------------
 * Creates a new buffer and returns it
 *
 * @args lap : the lap that the buffer represents
 *       num_bikers : the number of bikers in the sprint
 *
 * @return the newly Created buffer
 */
Buffer new_buffer(u_int lap, u_int num_bikers);

/*
 * Function: destroy_buffer
 * --------------------------------------------------------
 * Destroys the buffer, freeing the memory
 *
 * @args  b :  the buffer
 *
 * @return
 */
void destroy_buffer(Buffer b);

/*
 * Function: create_dummy_threads
 * --------------------------------------------------------
 * Create the global variable for the dummy threads
 *
 * @args numBikers : the number of bikers
 *
 * @return
 */
void create_dummy_threads(u_int numBikers);

/*
 * Function: destroy_dummy_threads
 * --------------------------------------------------------
 * Destroy the global dummy_thread structure. Should only be
 * called after all dummy threads that were created were
 * destroyed
 *
 * @args
 *
 * @return
 */
void destroy_dummy_threads();

int compareTo(const void*, const void*);

#endif
