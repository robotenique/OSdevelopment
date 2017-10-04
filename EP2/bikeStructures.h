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

#define NUM_LANES 10

// TODO: Think about the type of road the matrix...
// TODO: Think about the data structures of the biker...
typedef enum { false, true } bool;

typedef unsigned int u_int;

typedef unsigned long long int u_lint;

struct biker {
    // TODO: remove color after finishing EP
    // Speed = 2 (90 km/h) , 3 (60 km/h),  6 (30 km/h)
    u_int lap, i, j, id, score, speed;
    u_int lsp; // last sprint position
    char *color;
    u_lint localTime, totalTime;
    pthread_t *thread;
    pthread_mutex_t *mtxs;
    // In the end, to obtain the broken bikers
    // if broken == true, get the lap the biker broke
    bool broken;
};

struct score_s {
    u_int id, score;
};

struct buffer_s {
    struct score_s *data;
    u_int lap, i, size;
    pthread_mutex_t mtx;
    void(*append)(struct buffer_s*, u_int, u_int);
};

typedef struct buffer_s* Buffer;

struct scbr_s {
    Buffer *scores;
    u_int n;
    u_int tot_num_bikers, act_num_bikers;
    pthread_mutex_t scbr_mtx;
    void(*add_score)(struct scbr_s*, struct biker*);
};

typedef struct biker* Biker;
typedef struct scbr_s* Scoreboard;

typedef struct {
    pthread_mutex_t **mtxs;
    u_int **road;
    u_int length, lanes, laps;
} Road;


bool DEBUG_MODE;
Road speedway;
Scoreboard sb;
Biker *bikers;
Buffer broken;
pthread_barrier_t barr;
pthread_barrier_t debugger_barr;
pthread_barrier_t start_shot;

/*
 * Function: new_bikers
 * --------------------------------------------------------
 * Creates the bikers, the global array of bikers
 *
 * @args numBikers : the number of bikers
 *
 * @return
 */
void new_bikers(u_int numBikers);

/*
 * Function: destroy_bikers
 * --------------------------------------------------------
 * Destroys the bikers global array
 *
 * @args numBikers : number of bikers in the global array
 *
 * @return
 */
void destroy_bikers(u_int numBikers);

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
void create_speedway(u_int d, u_int laps);

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
 * Creates a new scoreboard, and returns it
 *
 * @args laps : The quantity of laps
 *       num_bikers : The number of bikers
 *
 * @return the new scoreboard
 */
Scoreboard new_scoreboard(u_int laps, u_int num_bikers);

/*
 * Function: destroy_scoreboard
 * --------------------------------------------------------
 * Destroys the scoreboard, freeing the memory
 *
 * @args sb : the scoreboard to be destroyed
 *
 * @return
 */
void destroy_scoreboard(Scoreboard sb);

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
 * Function: append
 * --------------------------------------------------------
 * Function to add a new id and score to the buffer_s
 *
 * @args b : the buffer
 *       id: the id of the biker to add
 *       score : the score of the biker to add
 *
 * @return
 */
void append(Buffer b, u_int id, u_int score);

/*
 * Function: biker_loop
 * --------------------------------------------------------
 * Base function for biker threads
 *
 * @args arg : biker informations
 *
 * @return
 */
void* biker_loop(void *arg);

#endif
