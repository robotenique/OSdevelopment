#include "biker.h"
#include "error.h"
#include "debugger.h"
#include "randomizer.h"

// TODO: In the end, wait for all threads to join

bool try_move(Biker self, u_int next_lane);
void calc_new_speed(Biker self);
void break_biker(Biker self);


static pthread_mutex_t broken_mtx;
//TODO: remove this thing (?)
static u_int color_num = 0;

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
    b->lsp = 0;
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
    b->try_move = &try_move;
    b->calc_new_speed = &calc_new_speed;
    pthread_create(b->thread, NULL, &biker_loop, (void*)b);
    return b;
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
    while (true) {
        moved = false;
        if (par%self->speed == 0) {
            i = self->i; // The current meter
            j = self->j; // The current lane
            u_int next_meter = (i + 1)%speedway.length;
            // The superior diagonal
            if (speedway.exists(next_meter, j - 1))
                moved = self->try_move(self, j - 1);
            // The lane just ahead
            if (!moved)
                moved = self->try_move(self, j);
            // The inferior diagonal
            if (!moved && speedway.exists(next_meter, j+1))
                moved = self->try_move(self, j+1);
            //if (!moved)
                //printf("Still %d %d %d %s\uf206%s\n", self->id, i, j, self->color, RESET);
        }
        par++;
        if (moved && self->i == 0) { // Trigger events each completed lap
            if (self->lap != -1) {
                sb->add_score(sb, self);
                self->calc_new_speed(self);
            }
            par = 1;
            (self->lap)++;
            if (self->lap%15 == 0 && event(0.01) && sb->tot_num_bikers > 5 && self->lap != 0) { // Break it down?
                break_biker(self);
                break;
            }
            if (self->lap == speedway.laps) {                
                speedway.road[self->i][self->j] = -1;
                dummy_threads->run_next(dummy_threads);
                break;
            }
        }
        printf("ESPERANDOOOOOOOOOOO 1\n");
        // Wait all other bikers move
        pthread_barrier_wait(&barr);
        printf("ESPERANDOOOOOOOOOOO 2\n");
        pthread_barrier_wait(&debugger_barr);
    }
    return NULL;
}

/*
 * Function: break_biker
 * --------------------------------------------------------
 * Break a biker D:. The biker is removed from the race,
 * added to the broken bikers buffer, and we call a
 * dummy_thread to aid him. Also, we must destroy the
 * thread of the biker.
 *
 * @args  self :  the biker that broke
 *
 * @return
 */
void break_biker(Biker self) {
    printf("Ciclista %u (%uº lugar na classificação) quebrou na volta %u\n", self->id, self->lsp, self->lap);
    self->broken = true;
    P(&(sb->scbr_mtx));
        sb->tot_num_bikers--;
    V(&(sb->scbr_mtx));
    speedway.road[self->i][self->j] = -1;
    P(&broken_mtx);
        broken->append(broken, self->id, self->score);
        dummy_threads->run_next(dummy_threads);
    V(&broken_mtx);
}

/*
 * Function: try_move
 * --------------------------------------------------------
 * Moves a biker on the road to the next row and to the "next_lane"
 * column
 *
 * @args self : the biker
 *       next_lane   : the next lane
 *
 * @return true if the biker were moved, false otherwise
 */
bool try_move(Biker self, u_int next_lane) {
    u_int i = self->i;
    u_int j = self->j;
    u_int next_meter = (i+1)%speedway.length;
    bool moved = false;
    //printf("%d Lock %d %d (Up)\n", self->id, next_meter, next_lane);
    // Lock the mutex of the next position
    P(&(speedway.mtxs[next_meter][next_lane]));
    // If the next position in the road is free
    if (speedway.road[next_meter][next_lane] == -1) {
        //printf("%d Lock %d %d (Self)\n", self->id, i, j);
        // Lock the mutex of the current position
        P(&(speedway.mtxs[i][j]));
        printf("id = %02d, next_meter = %02d, curr_lane = %02d, next_lane = %02d, %s\uf206%s\n", self->id, next_meter, j, next_lane, self->color, RESET);
        speedway.road[next_meter][next_lane] = self->id;
        speedway.road[i][j] = -1;
        self->i = next_meter;
        self->j = next_lane;
        moved = true;
        //printf("%d Unlock %d %d (Self)\n", self->id, i, j);
        V(&(speedway.mtxs[i][j]));
    }
    //printf("%d Unlock %d %d (Up)\n", self->id, next_meter, next_lane);
    V(&(speedway.mtxs[next_meter][next_lane]));

    return moved;
}

/*
 * Function: calc_new_speed
 * --------------------------------------------------------
 * Calculates the new speed of a biker
 *
 * @args self : the biker
 *
 * @return
 */
void calc_new_speed(Biker self) {
    if (self->speed == 6)
        self->speed = (event(0.7))? 3 : 6;
    else if (self->speed == 3)
        self->speed = (event(0.5))? 3 : 6;
}

void new_bikers(u_int numBikers) {
    pthread_mutex_init(&broken_mtx, NULL);
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
