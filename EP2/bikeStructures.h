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
    u_int lap, i, j, id, score, speed, color;
    u_lint localTime, totalTime;
    pthread_t *thread;
    pthread_cond_t *cond;
    pthread_mutex_t *mtx;
    // In the end, to obtain the broken bikers
    // if broken == true, get the lap the biker broke
    bool broken;
};

struct scoreboard {
    // TODO: Finish buffer
    // Every buffer need a mutex
    //Buffer *scoreList; // x = scoreList[i] -> x.lap
    u_int n;
    //add_score(biker)
};

bool DEBUG_MODE;
typedef struct biker* Biker;

typedef struct {
    u_int **road;
    u_int length;
    u_int lanes;
} Road;

Road speedway;

Biker *bikers;
pthread_barrier_t barr;

void new_bikers(u_int);
void new_road(u_int);

#endif
