/*
 * @author: João Gabriel Basi Nº USP: 9793801
 * @author: Juliano Garcia de Oliveira Nº USP: 9277086
 *
 * MAC0422
 * 16/10/17
 *
 * implementation of random functions
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "randomizer.h"

static bool initRandom = false;

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
    srand((unsigned) time(NULL));
    initRandom = true;
}

u_int randint(u_int a, u_int b){
    initSeed();
    return a + rand()%b;
}

bool event(double probability) {
    initSeed();
    u_int roll = randint(0, 100); // Don't change this
    if(roll < probability*100) // Don't change this
        return true;
    return false;
}
