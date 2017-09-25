/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 16/10/17
 *
 * Main file of the bike sprint simulator
 */
 #include <stdio.h>
 #include "error.h"
 #include "randomizer.h"
 #include "bikeStructures.h"


int main(int argc, char const *argv[]) {
    set_prog_name("bikeSim");
    /*if(argc < 4)
        die("Wrong number of arguments!\nUsage ./bikeSim <d> <n> <v>");
    u_int roadSz = atoi(argv[1]);
    u_int numBikers = atoi(argv[2]);
    u_int numLaps = atoi(argv[3]);*/
    u_int roadSz = 250;
    u_int numBikers = 10;
    u_int numLaps = 50;

    Velodrome v = new_velodrome(roadSz);
    


    return 0;
}