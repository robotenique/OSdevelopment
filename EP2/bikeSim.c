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
 #include "bikeStructures.h"
 #include "error.h"
 #include "randomizer.h"
 #include "debugger.h"


int main(int argc, char const *argv[]) {
    set_prog_name("bikeSim");
    /*if(argc < 5)
        die("Wrong number of arguments!\nUsage ./bikeSim <d> <n> <v> <debug>");
    u_int roadSz = atoi(argv[1]);
    u_int numBikers = atoi(argv[2]);
    u_int numLaps = atoi(argv[3]);
    if (argc == 4)
        DEBUG_MODE = true;
    else
        DEBUG_MODE = false;*/
    u_int roadSz = 100;
    u_int numBikers = 10;
    u_int numLaps = 50;
    DEBUG_MODE = true;

    new_road(roadSz);
    debug_road();



    return 0;
}
