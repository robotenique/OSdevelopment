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
    char *color;
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
    pthread_mutex_t **mtxs;
    u_int length;
    u_int lanes;
} Road;

Road speedway;
Scoreboard sb;

Biker *bikers;
pthread_barrier_t barr;
pthread_barrier_t barr2;

void new_bikers(u_int numBikers);
void create_speedway(u_int d);
void destroy_speedway();

Scoreboard new_scoreboard(u_int laps, u_int num_bikers);
void add_info(Buffer b, Biker x);
void add_score(Scoreboard sb, Biker x);
void destroy_scoreboard(Scoreboard sb);

Buffer new_buffer(u_int lap, u_int num_bikers);
void destroy_buffer(Buffer b);
void append(Buffer b, char *s);

void* biker_loop(void *arg);

#endif
