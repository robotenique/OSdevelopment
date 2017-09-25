#include "error.h"
#include "bikeStructures.h"

Velodrome new_velodrome(u_int d){
    Velodrome vel = emalloc(sizeof(struct velodrome));
    vel->lap = 0;
    vel->road = emalloc(d*sizeof(u_int*));
    for (int i = 0; i < d; i++)
        vel->road[i] = emalloc(10*sizeof(u_int));

    // Set all bikers to -1 (no bikers in the road)
    for (int i = 0; i < d; i++)
        for(int j = 0; j < 10; j++)
            vel->road[i][j] = -1;
    return vel;
}
