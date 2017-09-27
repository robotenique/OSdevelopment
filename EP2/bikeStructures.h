#ifndef __BIKE_STRUCTURES_H__
#define __BIKE_STRUCTURES_H__
#include <stdio.h>
#include <pthread.h>

// TODO: Think about the type of road the matrix...
// TODO: Think about the data structures of the biker...
typedef enum { false, true } bool;

typedef unsigned int u_int;

typedef unsigned long long int u_lint;
struct biker {
    // Speed = 2 (90 km/h) , 3 (60 km/h),  6 (30 km/h)
    u_int lap, i, j, id, score, speed;
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
    Buffer *scoreList; // x = scoreList[i] -> x.lap
    u_int n;
    //add_score(biker)
};

bool DEBUG_MODE = false;
typedef struct biker* Biker;
u_int **road;
Biker *bikers;
pthread_barrier_t barr;

void new_bikers(u_int);
void new_road(u_int);

#endif
