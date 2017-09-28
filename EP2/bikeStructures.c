#include "error.h"
#include "bikeStructures.h"


void new_road(u_int d){
    speedway.road = emalloc(d*sizeof(u_int*));
    speedway.length = d;
    speedway.lanes = NUM_LANES;
    for (int i = 0; i < d; i++)
        speedway.road[i] = emalloc(10*sizeof(u_int));
    for (int i = 0; i < d; i++)
        for(int j = 0; j < 10; j++)
            speedway.road[i][j] = -1; // infinity :'(
}
