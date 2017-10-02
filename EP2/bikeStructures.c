/*
 * @author: João Gabriel Basi Nº USP: 9793801
 * @author: Juliano Garcia de Oliveira Nº USP: 9277086
 *
 * MAC0422
 * 16/10/17
 *
 * This file contains the implementation and logic of the bikers.
 * The functions that deal with the scoreboard, the speedway,
 * the bikers, etc. They're all implemented here.
 */
#include <pthread.h>
#include "error.h"
#include "bikeStructures.h"
#include "debugger.h"

void add_score(Scoreboard, Biker);

/*
 * Function: create_speedway
 * --------------------------------------------------------
 * Create the global speedway. The speedway is a global variable
 * which has the road matrix (d x 10 matrix), the length is d,
 * and the lanes are (by default) 10.
 *
 * @args d : u_int which represents the length of the speedway
 *
 * @return
 */
void create_speedway(u_int d){
    speedway.road = emalloc(d*sizeof(u_int*));
    speedway.length = d;
    speedway.lanes = NUM_LANES;
    for (int i = 0; i < d; i++) {
        speedway.road[i] = emalloc(10*sizeof(u_int));
        for(int j = 0; j < 10; j++)
            speedway.road[i][j] = -1; // infinity :'(
    }
    speedway.mtxs = emalloc(d*sizeof(pthread_mutex_t*));
    for (int i = 0; i < d; i++) {
        speedway.mtxs[i] = emalloc(10*sizeof(pthread_mutex_t));
        for(int j = 0; j < 10; j++)
            pthread_mutex_init(&(speedway.mtxs[i][j]), NULL);
    }
}

/*
 * Function: destroy_speedway
 * --------------------------------------------------------
 * Destroy the global speedway, freeing the memory
 *
 * @args
 *
 * @return
 */
void destroy_speedway() {
    for (int i = 0; i < speedway.length; i++) {
        free(speedway.road[i]);
        free(speedway.mtxs[i]);
    }
    free(speedway.road);
    free(speedway.mtxs);
}

/*
 * Function: new_scoreboard
 * --------------------------------------------------------
 * Creates a new scoreboard, and returns it
 *
 * @args laps : The quantity of laps
 *       num_bikers : The number of bikers
 *
 * @return the new scoreboard
 */
Scoreboard new_scoreboard(u_int laps, u_int num_bikers) {
    u_int init_sz = 2 + (laps - 1)/4;
    Scoreboard sb = emalloc(sizeof(struct scbr_s));
    sb->scores = emalloc(init_sz*sizeof(Buffer));
    for (size_t i = 0; i < init_sz; sb->scores[i++] = NULL);
    sb->n = init_sz;
    sb->num_bikers = num_bikers;
    pthread_mutex_init(&(sb->scbr_mtx), NULL);
    sb->add_score = &add_score;
    return sb;
}

/*
 * Function: reallocate_scoreboard
 * --------------------------------------------------------
 * Auxiliar function to reallocate the internal buffer array,
 * remapping every old buffer to a new position, then returning
 * the position of doubling the size.
 *
 * @args sb : the scoreboard
 * @args x  : The biker to insert the array
 *
 * @return  the new position for the buffer
 */
u_int reallocate_scoreboard(Scoreboard sb, Biker x) {
    u_int new_sz = sb->n * 2;
    Buffer *temp = emalloc(new_sz*sizeof(Buffer));
    for (size_t i = 0; i < sb->n; i++) {
        if(sb->scores[i] != NULL) {
            u_int new_pos = sb->scores[i]->lap % new_sz;
            temp[new_pos] = sb->scores[i];
        }
    }
    sb->scores = temp;
    sb->n = new_sz;

    return x->lap % sb->n;
}

/*
 * Function: add_score
 * --------------------------------------------------------
 * Add a new score to the Scoreboard, sending the new biker
 *
 * @args sb : the scoreboard
 *       x  : the biker
 *
 * @return
 */
void add_score(Scoreboard sb, Biker x) {
    /* TODO: when a player is the last one to add, it
        needs to print out the information of everything..*/
    pthread_mutex_lock(&(sb->scbr_mtx));
    u_int pos = x->lap % sb->n;
    // TODO: LOCK A MUTEX of this position... maybe it's
    // better to keep the mtx list in the Scoreboard struct...
    Buffer currb = sb->scores[pos];
    if(currb && currb->lap != x->lap)
        pos = reallocate_scoreboard(sb, x);
    pthread_mutex_unlock(&(sb->scbr_mtx));
    if(currb == NULL)
        currb = new_buffer(x->lap, sb->num_bikers);
    currb->append(currb, x->id);
    debug_buffer(currb);
    x->lap++;
    if(currb->i == sb->num_bikers) {
        printf("meh\n"); // TODO: Delete this buffer and print everything...
        debug_buffer(currb);
        destroy_buffer(currb);
        currb = NULL;
    }
}

/*
 * Function: destroy_scoreboard
 * --------------------------------------------------------
 * Destroys the scoreboard, freeing the memory
 *
 * @args sb : the scoreboard to be destroyed
 *
 * @return
 */
void destroy_scoreboard(Scoreboard sb) {
    for (int i = 0; i < sb->n; i++) {
        if (sb->scores[i] != NULL)
            destroy_buffer(sb->scores[i]);
    }
    free(sb->scores);
    free(sb);
}

/*
 * Function: new_buffer
 * --------------------------------------------------------
 * Creates a new buffer and returns it
 *
 * @args lap : the lap that the buffer represents
 *       num_bikers : the number of bikers in the sprint
 *
 * @return the newly Created buffer
 */
Buffer new_buffer(u_int lap, u_int num_bikers) {
    Buffer b = emalloc(sizeof(struct buffer_s*));
    b->lap = lap;
    b->i = 0;
    b->data = emalloc(num_bikers*sizeof(u_int));
    b->size = num_bikers;
    b->append = &append;
    pthread_mutex_init(&(b->mtx), NULL);
    return b;
}

/*
 * Function: append
 * --------------------------------------------------------
 * Function to add a new id to the buffer_s
 *
 * @args b : the buffer
 *       id: the id of the biker to add
 *
 * @return
 */
void append(Buffer b, u_int id) {
    pthread_mutex_lock(&(b->mtx));
    b->data[b->i] = id;
    b->i++;
    pthread_mutex_unlock(&(b->mtx));
}

/*
 * Function: destroy_buffer
 * --------------------------------------------------------
 * Destroys the buffer, freeing the memory
 *
 * @args  b :  the buffer
 *
 * @return
 */
void destroy_buffer(Buffer b){
    free(b->data);
    free(b);
}

/*
 * Function: new_biker
 * --------------------------------------------------------
 * Creates a new biker with the given id.
 *
 * @args id : the id of the biker
 *
 * @return the newly created biker
 */
Biker new_biker(u_int id) {
    Biker b = emalloc(sizeof(struct biker));
    b->lap = 0;
    b->id = id;
    b->score = 0;
    b->speed = 0;
    b->color = estrdup("\x1b[31m");
    b->thread = emalloc(sizeof(pthread_t));
    b->mtxs = emalloc(3*sizeof(pthread_mutex_t));
    u_int meter = (speedway.length - id/speedway.lanes)%speedway.length;
    u_int lane = id%speedway.lanes;
    speedway.road[meter][lane] = id;
    b->i = meter;
    b->j = lane;
    pthread_create(b->thread, NULL, &biker_loop, (void*)b);
    return b;
}

/*
 * Function: new_bikers
 * --------------------------------------------------------
 * Creates the bikers, the global array of bikers
 *
 * @args numBikers : the number of bikers
 *
 * @return
 */
void new_bikers(u_int numBikers) {
    bikers = emalloc(numBikers*sizeof(Biker));
    for (int i = 0; i < numBikers; i++)
        bikers[i] = new_biker(i);
}

/*
 * Function: destroy_bikers
 * --------------------------------------------------------
 * Destroys the bikers global array
 *
 * @args numBikers : number of bikers in the global array
 *
 * @return
 */
void destroy_bikers(u_int numBikers) {
    for (size_t i = 0; i < numBikers; i++) {
        Biker b = bikers[i];
        if(b != NULL) {
            free(b->thread);
            free(b->mtxs);
            free(b->color);
            free(b);
        }
    }
    free(bikers);
}

/*
 * Function: exists
 * --------------------------------------------------------
 * Returns if a position in the speedway exists
 *
 * @args i : the row
 *       j : the column
 *
 * @return
 */
bool exists(int i, int j) {
    return (i >= 0 && i < speedway.length && j >= 0 && j < speedway.lanes);
}

bool move(Biker self, u_int nj) {
    u_int i = self->i;
    u_int j = self->j;
    u_int im = (i+1)%speedway.length;
    bool moved = false;
    //printf("%d Lock %d %d (Up)\n", self->id, im, nj);
    pthread_mutex_lock(&(speedway.mtxs[im][nj]));
    if (speedway.road[im][nj] == -1) {
        //printf("%d Lock %d %d (Self)\n", self->id, i, j);
        pthread_mutex_lock(&(speedway.mtxs[i][j]));
        //printf("Up %d %d %d %s\uf206%s\n", self->id, im, j, self->color, RESET);
        speedway.road[im][nj] = self->id;
        speedway.road[i][j] = -1;
        self->i = im;
        self->j = nj;
        moved = true;
        //printf("%d Unlock %d %d (Self)\n", self->id, i, j);
        pthread_mutex_unlock(&(speedway.mtxs[i][j]));
    }
    //printf("%d Unlock %d %d (Up)\n", self->id, im, nj);
    pthread_mutex_unlock(&(speedway.mtxs[im][nj]));
    return moved;
}

void* biker_loop(void *arg) {
    Biker self = (Biker)arg;
    // TODO: Change this to a while true
    bool up, moved;
    int i, j;
    for (int k = 0; k < 20; k++) {
        up = false;
        moved = false;
        i = self->i;
        j = self->j;
        if (exists(i, j+1)) {
            //printf("%d Lock %d %d (Same up)\n", self->id, i, j+1);
            pthread_mutex_lock(&(speedway.mtxs[i][j+1]));
            if (speedway.road[i][j+1] != -1)
                up = true;
            //printf("%d Unlock %d %d (Same up)\n", self->id, i, j+1);
            pthread_mutex_unlock(&(speedway.mtxs[i][j+1]));
        }
        if (!up) {
            u_int im = (i+1)%speedway.length;
            if (exists(im, j-1)) {
                moved = move(self, j-1);
            }
            if (!moved && exists(im, j)) {
                moved = move(self, j);
            }
            if (!moved && exists(im, j+1)) {
                moved = move(self, j+1);
            }
        }
        //if (!moved)
            //printf("Still %d %d %d %s\uf206%s\n", self->id, i, j, self->color, RESET);
        //printf("%d waiting...\n", self->id);
        pthread_barrier_wait(&barr);
        if (moved && self->i == 0)
            sb->add_score(sb, self);
        //printf("%d waiting 2...\n", self->id);
        pthread_barrier_wait(&barr2);
    }
    return NULL;
}
