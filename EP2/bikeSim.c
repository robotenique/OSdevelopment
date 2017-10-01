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

void debug_buffer(Buffer b) {
    printf("LAP = %u, i = %u, SIZE = %u\n", b->lap, b->i, b->size);
    for (size_t i = 0; i < b->i; i++)
        printf("%s\n", b->data[i]);

}


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
    u_int roadSz = 80;
    u_int numBikers = 10;
    u_int numLaps = 50;
    DEBUG_MODE = true;

    create_speedway(roadSz);
    random_initialize(numBikers);
    printf("bikers criados\n");
    debug_road();
    Biker bike0 = emalloc(sizeof(struct biker));
    bike0->lap = 0;
    bike0->id = 0;
    bike0->score = 0;
    Scoreboard sb = new_scoreboard(20, 10);
    for (int i = 0; i < 20; i++) {
        add_score(sb, bike0);
        debug_buffer(sb->scores[i]);
        bike0->lap++;
    }

    return 0;
}
