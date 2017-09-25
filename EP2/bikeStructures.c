#include "error.h"
#include "bikeStructures.h"

Velodrome new_velodrome(u_int d){
    Velodrome vel = emalloc(sizeof(struct velodrome));
    vel->road = emalloc(d*sizeof(Biker*));
    for (int i = 0; i < d; i++)
        vel->road[i] = emalloc(10*sizeof(Biker));
    for (int i = 0; i < d; i++)
        for(int j = 0; j < 10; j++)
            vel->road[i][j] = NULL;
    return vel;
}
