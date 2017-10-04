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
#include "randomizer.h"

//TODO: remove this thing
static u_int color_num = 0;
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
    if(sb->scores[pos] && sb->scores[pos]->lap != x->lap)
        pos = reallocate_scoreboard(sb, x);
    if(sb->scores[pos] == NULL)
        sb->scores[pos] = new_buffer(x->lap, sb->num_bikers);
    pthread_mutex_unlock(&(sb->scbr_mtx));
    sb->scores[pos]->append(sb->scores[pos], x);
    debug_buffer(sb->scores[pos]);
    x->lap++;
    if(sb->scores[pos]->i == sb->num_bikers) {
        printf("meh\n"); // TODO: Delete this buffer and print everything...
        debug_buffer(sb->scores[pos]);
        destroy_buffer(sb->scores[pos]);
        sb->scores[pos] = NULL;
    }
}

void append(Buffer b, Biker x) {
    pthread_mutex_lock(&(b->mtx));
    b->data[b->i].id = x->id;
    b->data[b->i].score = x->score;
    b->i++;
    pthread_mutex_unlock(&(b->mtx));
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
    b->lap = (id < 10)? 0 : -1;
    b->id = id;
    b->score = 0;
    b->speed = 6;
    b->color = estrdup(get_color(color_num++));
    b->thread = emalloc(sizeof(pthread_t));
    b->mtxs = emalloc(3*sizeof(pthread_mutex_t));
    for (int i = 0; i < 3; i++)
        pthread_mutex_init(&(b->mtxs[i]), NULL);
    // Start the first row of bikers just after the starting line (meter 0) and
    // the other rows, behind them
    u_int meter = (speedway.length - id/speedway.lanes)%speedway.length;
    u_int lane = id%speedway.lanes;
    speedway.road[meter][lane] = id;
    b->i = meter;
    b->j = lane;
    pthread_create(b->thread, NULL, &biker_loop, (void*)b);
    return b;
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

/*
 * Function: move
 * --------------------------------------------------------
 * Moves a biker on the road to the next row and to the "nj"
 * column
 *
 * @args self : the biker
 *       next_lane   : the next lane
 *
 * @return true if the biker were moved, false otherwise
 */
bool move(Biker self, u_int next_lane) {
    u_int i = self->i;
    u_int j = self->j;
    u_int next_meter = (i+1)%speedway.length;
    bool moved = false;
    //printf("%d Lock %d %d (Up)\n", self->id, next_meter, next_lane);
    // Lock the mutex of the next position
    pthread_mutex_lock(&(speedway.mtxs[next_meter][next_lane]));
    // If the next position in the road is free
    if (speedway.road[next_meter][next_lane] == -1) {
        //printf("%d Lock %d %d (Self)\n", self->id, i, j);
        // Lock the mutex of the current position
        pthread_mutex_lock(&(speedway.mtxs[i][j]));
        printf("id = %02d, next_meter = %02d, curr_lane = %02d, next_lane = %02d, %s\uf206%s\n", self->id, next_meter, j, next_lane, self->color, RESET);
        speedway.road[next_meter][next_lane] = self->id;
        speedway.road[i][j] = -1;
        self->i = next_meter;
        self->j = next_lane;
        moved = true;
        //printf("%d Unlock %d %d (Self)\n", self->id, i, j);
        pthread_mutex_unlock(&(speedway.mtxs[i][j]));
    }
    //printf("%d Unlock %d %d (Up)\n", self->id, next_meter, next_lane);
    pthread_mutex_unlock(&(speedway.mtxs[next_meter][next_lane]));

    return moved;
}

void calc_new_speed(Biker self) {
    if (self->speed == 6)
        self->speed = (event(0.7))? 3 : 6;
    else if (self->speed == 3)
        self->speed = (event(0.5))? 3 : 6;
}

void* biker_loop(void *arg) {
    Biker self = (Biker)arg;
    bool moved = false;
    u_int i, j;
    u_int par = 0; // Parity of the biker
    pthread_barrier_wait(&start_shot);
    /* The biker (0) can make the following movements:
     *
     *          +----+----+----+----+----+
     *          |    |    | 1 1|    |    |
     *          |    |    | 1 1|    |    |
     *          +------------------------+
     *          |    | 0 0| 2 2|    |    |
     *          |    | 0 0| 2 2|    |    |
     *          +------------------------+
     *          |    |    | 3 3|    |    |
     *          |    |    | 3 3|    |    |
     *          +----+----+----+----+----+
     * The 1's are the superior diagonal, the 2's the position ahead
     * and the 3's the inferior diagonal.
     */
     // TODO: Change this to a while true
    for (int k = 0; k < 500; k++) {
        moved = false;
        if (par%self->speed == 0) {
            i = self->i; // The current meter
            j = self->j; // The current lane
            u_int next_meter = (i + 1)%speedway.length;
            // The superior diagonal
            if (exists(next_meter, j - 1))
                moved = move(self, j - 1);
            // The lane just ahead
            if (!moved)
                moved = move(self, j);
            // The inferior diagonal
            if (!moved && exists(next_meter, j+1))
                moved = move(self, j+1);
            //if (!moved)
                //printf("Still %d %d %d %s\uf206%s\n", self->id, i, j, self->color, RESET);
        }
        par++;
        if (moved && self->i == 0) {
            if (self->lap != -1) { // If the biker completed a lap
                sb->add_score(sb, self);
                par = 1;
                calc_new_speed(self);
            }
            else
                (self->lap)++;
        }
        // Wait all other bikers move
        //printf("%d waiting...\n", self->id);
        pthread_barrier_wait(&barr);
        //printf("%d waiting 2...\n", self->id);
        pthread_barrier_wait(&debugger_barr);
    }
    return NULL;
}

void create_speedway(u_int d, u_int laps) {
    speedway.road = emalloc(d*sizeof(u_int*));
    speedway.length = d;
    speedway.lanes = NUM_LANES;
    speedway.laps = laps;
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

void destroy_speedway() {
    for (int i = 0; i < speedway.length; i++) {
        free(speedway.road[i]);
        free(speedway.mtxs[i]);
    }
    free(speedway.road);
    free(speedway.mtxs);
}

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

void destroy_scoreboard(Scoreboard sb) {
    for (int i = 0; i < sb->n; i++) {
        if (sb->scores[i] != NULL)
            destroy_buffer(sb->scores[i]);
    }
    free(sb->scores);
    free(sb);
}

Buffer new_buffer(u_int lap, u_int num_bikers) {
    Buffer b = emalloc(sizeof(struct buffer_s));
    b->data = emalloc(num_bikers*sizeof(struct score_s));
    b->lap = lap;
    b->i = 0;
    b->size = num_bikers;
    b->append = &append;
    pthread_mutex_init(&(b->mtx), NULL);
    return b;
}

void destroy_buffer(Buffer b){
    free(b->data);
    free(b);
}

void new_bikers(u_int numBikers) {
    bikers = emalloc(numBikers*sizeof(Biker));
    for (int i = 0; i < numBikers; i++)
        bikers[i] = new_biker(i);
}

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
