#ifndef __BIKE_STRUCTURES_H__
#define __BIKE_STRUCTURES_H__

// TODO: Think about the type of road the matrix...
// TODO: Think about the data structures of the biker...
typedef enum { false, true } bool;

typedef unsigned int u_int;

struct biker {
    u_int id;
};

typedef struct biker* Biker;

struct velodrome{
    u_int **road;
    u_int lap;
};

typedef struct velodrome* Velodrome;


Velodrome new_velodrome(u_int);

#endif
