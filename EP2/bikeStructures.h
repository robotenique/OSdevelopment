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

struct buffer_s{
    u_int lap, i, size;
    char **data;
    pthread_mutex_t mtx;
    void(*append)(struct buffer_s*, char*);
};

typedef struct buffer_s* Buffer;

struct scbr_s {
    // TODO: Every buffer need a mutex
    Buffer *scores;
    u_int n;
    u_int num_bikers;
    //add_score(biker)
};

bool DEBUG_MODE;

typedef struct biker* Biker;
typedef struct scbr_s* Scoreboard;

typedef struct {
    u_int **road;
    u_int length;
    u_int lanes;
} Road;

Road speedway;

Biker *bikers;
pthread_barrier_t barr;

void new_bikers(u_int numBikers);
void create_speedway(u_int d);

Scoreboard new_scoreboard(u_int laps, u_int num_bikers);
void add_info(Buffer b, Biker x);
void add_score(Scoreboard sb, Biker x);


Buffer new_buffer(u_int lap);
void destroy_buffer(Buffer b);
void append(Buffer b, char *s);


#endif
