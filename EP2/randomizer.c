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
#include <pthread.h>
#include "randomizer.h"
#include "macros.h"

static bool initRandom = false;
static pthread_mutex_t rmtx = PTHREAD_MUTEX_INITIALIZER;

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
    P(rmtx);
    initSeed();
    u_int ret = a + rand()%b;
    V(rmtx);
    return ret;
}

bool event(double probability) {
    P(rmtx);
    initSeed();
    u_int roll = randint(0, 100);
    V(rmtx);
    if(roll < probability*100)
        return true;
    return false;
}
