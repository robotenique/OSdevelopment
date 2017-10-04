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
#include <stdlib.h>
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
 * Function: compareTo
 * --------------------------------------------------------
 * Compare two score_s
 *
 * @args a : the first one
 *       b : the second one
 *
 * @return positive if a > b, zero if a == b, negative if a < b
 */
int compareTo(const void *a, const void *b) {
    return ((struct score_s*)b)->score - ((struct score_s*)a)->score;
}

/*
 * Function: print_buffer
 * --------------------------------------------------------
 * Formatted print of the buffer
 *
 * @args b : the buffer
 *
 * @return
 */
void print_buffer(Buffer b) {
    printf("Relatório da volta %u\n", b->lap + 1);
    for (size_t i = 0; i < b->i; i++)
        printf("%luº - Biker %u\n", i+1, b->data[i].id);
    if (b->lap%10 == 0) {
        qsort(b->data, b->i, sizeof(struct score_s), &compareTo);
        for (size_t i = 0; i < b->i; i++)
            printf("%luº - Biker %u - %upts\n", i+1, b->data[i].id, b->data[i].score);
    }
    if (b->lap == speedway.laps - 1) {
        qsort(b->data, b->i, sizeof(struct score_s), &compareTo);
        qsort(broken->data, broken->i, sizeof(struct score_s), &compareTo);
        for (size_t i = 0; i < b->i; i++)
            // TODO : Add time to this print
            printf("%luº - Biker %u - %upts\n", i+1, b->data[i].id, b->data[i].score);
        for (size_t i = 0; i < broken->i; i++) {
            Biker x = bikers[broken->data[i].id];
            printf("Quebrou na volta %u - Biker %u - %upts\n", x->lap, x->id, x->score);
        }
    }
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
        sb->scores[pos] = new_buffer(x->lap, sb->tot_num_bikers);
    Buffer prev_b = sb->scores[(pos - 1 + sb->n)%sb->n];
    Buffer b = sb->scores[pos];
    if (prev_b != NULL && prev_b->lap + 1 == b->lap && prev_b->i == 1)
        x->score += 20;
    pthread_mutex_unlock(&(sb->scbr_mtx));
    pthread_mutex_lock(&(b->mtx));
    b->append(b, x->id, x->score);
    if (b->lap%10 == 0 && b->i <= 4) {
        if (b->i == 1) x->score += 5;
        else if (b->i == 2) x->score += 3;
        else if (b->i == 3) x->score += 2;
        else if (b->i == 4) x->score += 1;
    }
    pthread_mutex_unlock(&(b->mtx));
    debug_buffer(b);
    //printf("b->i = %d == bikers = %d\n", b->i, sb->tot_num_bikers);
    if(b->i == sb->tot_num_bikers) {
        printf("meh\n"); // TODO: Delete this buffer and print everything...
        print_buffer(b);
        destroy_buffer(sb->scores[pos]);
        sb->scores[pos] = NULL;
    }
}

void append(Buffer b, u_int id, u_int score) {
    b->data[b->i].id = id;
    b->data[b->i].score = score;
    b->i++;
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

/*
 * Function: dummy
 * --------------------------------------------------------
 * Dummy thread function
 *
 * @args
 *
 * @return
 */
void* dummy(void *arg) {
    while (1) { // TODO : Stop this while true
        pthread_barrier_wait(&barr);
        pthread_barrier_wait(&debugger_barr);
    }
    return NULL;
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
    while (true) {
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
        if (moved && self->i == 0) { // Trigger events each completed lap
            if (self->lap != -1) {
                sb->add_score(sb, self);
                calc_new_speed(self);
            }
            par = 1;
            (self->lap)++;
            if (self->lap%15 == 0 && event(0.01) && sb->tot_num_bikers > 5) { // Break it down?
                printf("Ciclista %u (%uº lugar na classificação) quebrou na volta %u\n", self->id, self->lsp, self->lap);
                self->broken = true;
                pthread_mutex_lock(&(sb->scbr_mtx));
                sb->act_num_bikers--;
                sb->tot_num_bikers--;
                pthread_mutex_unlock(&(sb->scbr_mtx));
                speedway.road[self->i][self->j] = -1;
                broken->append(broken, self->id, self->score);
                pthread_create(self->thread, NULL, &dummy, NULL);
                break;
            }
            if (self->lap == speedway.laps) {
                pthread_mutex_lock(&(sb->scbr_mtx));
                sb->act_num_bikers--;
                pthread_mutex_unlock(&(sb->scbr_mtx));
                speedway.road[self->i][self->j] = -1;
                pthread_create(&(dummies[self->id]), NULL, &dummy, NULL);
                break;
            }
        }
        // Wait all other bikers move
        pthread_barrier_wait(&barr);
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
    sb->tot_num_bikers = num_bikers;
    sb->act_num_bikers = num_bikers;
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
