#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "randomizer.h"

static bool initRandom = false;

void initSeed();


u_int randint(u_int a, u_int b){
    initSeed();
    return a + rand()%b;
}

bool event(double probability) {
    initSeed();
    u_int roll = randint(0, 101);
    if(roll <= probability*100)
        return true;
    return false;
}

/*
 * Function: initSeed
 * --------------------------------------------------------
 * Auxiliar function to initialize the random seed.
 * Must be called exactly once to prevent problems...
 *
 * @args
 *
 * @return
 */
void initSeed(){
    if(initRandom)
        return;
    printf("Seed foi inicializada...\n");
    srand((unsigned) time(NULL));
    initRandom = true;
}
