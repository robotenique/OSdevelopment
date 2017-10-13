#ifndef __BIKER_H__
#define __BIKER_H__
#include <pthread.h>
#include "macros.h"

typedef enum { NONE, TOP, DOWN, TOPDOWN } Move;

struct biker {
    // TODO: remove color after finishing EP (or NOT!)
    // Speed = 2 (90 km/h) , 3 (60 km/h),  6 (30 km/h)
    u_int lap, i, j, id, score, speed;
    bool fast, moved, *used_mtx;
    char *color;
    u_lint totalTime;
    Move moveType;
    pthread_t *thread;
    pthread_mutex_t *mtxs;
    bool (*try_move)(struct biker* self, u_int next_lane);
    void(*calc_new_speed)(struct biker* self);

    // In the end, to obtain the broken bikers
    // if broken == true, get the lap the biker broke
    bool broken;
};

typedef struct biker* Biker;

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

#endif
