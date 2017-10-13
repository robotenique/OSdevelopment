/*
 * @author: João Gabriel Basi Nº USP: 9793801
 * @author: Juliano Garcia de Oliveira Nº USP: 9277086
 *
 * MAC0422
 * 16/10/17
 *
 * File for biker functions
 */
#include "biker.h"
#include "error.h"
#include "debugger.h"
#include "randomizer.h"

bool try_move(Biker self, u_int next_lane);
void calc_new_speed(Biker self);
void break_biker(Biker self);
void destroy_all();

typedef enum {BROKEN, FINISHED, NORMAL} Status;

char* getspeed(Biker self) {

  if (self->speed == 2)
    return estrdup("\x1b[38;5;240m90 km/h\x1b[0m");
  else if(self->speed == 3)
    return estrdup("\x1b[38;5;214m60 km/h\x1b[0m");
  else if(self->speed == 6)
    return estrdup("\x1b[38;5;105m30 km/h\x1b[0m");
  else
    return estrdup("QUE??");
}

static pthread_mutex_t broken_mtx;
//TODO: remove this thing (?)
static u_int color_num = 0;

// TODO: Remove the function below
char* getsymbol(int i){
    if (i == 0)
        return estrdup("↖");
    else if(i == 1)
        return estrdup("←");
    else if(i == 2)
        return estrdup("↙");
    else if(i == 3)
        return estrdup("↓");
    die("ERROR!");
    return estrdup("Error!");
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
    b->lap = (id < NUM_LANES)? 0 : -1;
    b->id = id;
    b->score = 0;
    b->speed = 6;
    b->moved = false;
    b->totalTime = 0;
    b->fast = false;
    b->moveType = TOPDOWN;
    b->color = estrdup(get_color((color_num++)%230));
    b->thread = emalloc(sizeof(pthread_t));
    /* What each mutex is referring to (X is the biker):
     *    0
     *    1  X
     *    2  3
     */
    // 0 -> top left, 1 -> left, 2 -> bottom left, 3 -> bottom
    b->mtxs = emalloc(4*sizeof(pthread_mutex_t));
    b->used_mtx = emalloc(4*sizeof(bool));
    // Set all mutexes to 0 at first
    for (int i = 0; i < 4; i++) {
        pthread_mutex_init(&(b->mtxs[i]), NULL);
        P(&(b->mtxs[i]));
        b->used_mtx[i] = false;
    }
    // Start the first row of bikers just after the starting line (meter 0) and
    // the other rows, behind them
    u_int meter = (speedway.length - id/speedway.lanes)%speedway.length;
    u_int lane = id%speedway.lanes;
    speedway.road[meter][lane] = id;
    b->i = meter;
    b->j = lane;
    speedway.nbpl[lane]++;
    b->try_move = &try_move;
    b->calc_new_speed = &calc_new_speed;
    pthread_create(b->thread, NULL, &biker_loop, (void*)b);
    pthread_detach(*(b->thread));
    return b;
}

void* biker_loop(void *arg) {
    Biker self = (Biker)arg;
    bool moved = false;
    u_int i, j, mem;
    u_lint par = 0; // Parity of the biker
    i = self->i; // The current meter
    j = self->j; // The current lane
    u_int next_meter = (i + 1)%speedway.length;
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
        Status biker_status = NORMAL;
        moved = false;
        if (par%self->speed == 0) {
            // The superior diagonal
            if (speedway.exists(i, j - 1) && (self->moveType & TOP) && speedway.nbpl[j-1] < speedway.length-1 && (mem = speedway.road[next_meter][j-1]) != -1 && !(bikers[mem]->moved)) {
                bikers[mem]->used_mtx[0] = true;
                //printf("--> %sBiker %d%s locked %s%d%s\n", self->color, self->id, RESET, bikers[mem]->color, mem, RESET);
                P(&(bikers[mem]->mtxs[0]));
                //printf("<-- %sBiker %d%s proceed\n", self->color, self->id, RESET);
            }
            // The meter just ahead
            if ((mem = speedway.road[next_meter][j]) != -1 && !(bikers[mem]->moved)) {
                bikers[mem]->used_mtx[1] = true;
                //printf("--> %sBiker %d%s locked %s%d%s\n", self->color, self->id, RESET, bikers[mem]->color, mem, RESET);
                P(&(bikers[mem]->mtxs[1]));
                //printf("<-- %sBiker %d%s proceed\n", self->color, self->id, RESET);
            }
            // The inferior diagonal
            if (speedway.exists(i, j + 1) && (self->moveType & DOWN) && speedway.nbpl[j+1] < speedway.length-1 && (mem = speedway.road[next_meter][j+1]) != -1 && !(bikers[mem]->moved)) {
                bikers[mem]->used_mtx[2] = true;
                //printf("--> %sBiker %d%s locked %s%d%s\n", self->color, self->id, RESET, bikers[mem]->color, mem, RESET);
                P(&(bikers[mem]->mtxs[2]));
                //printf("<-- %sBiker %d%s proceed\n", self->color, self->id, RESET);
            }
            // The lane just above
            if (speedway.exists(i, j - 1) && speedway.nbpl[j-1] < speedway.length-1 && (mem = speedway.road[i][j - 1]) != -1 && !(bikers[mem]->moved)) {
                bikers[mem]->used_mtx[3] = true;
                //printf("--> %sBiker %d%s locked %s%d%s\n", self->color, self->id, RESET, bikers[mem]->color, mem, RESET);
                P(&(bikers[mem]->mtxs[3]));
                //printf("<-- %sBiker %d%s proceed\n", self->color, self->id, RESET);
            }
            //printf("Moving %sbiker %d%s\n", self->color, self->id, RESET);
            if (speedway.exists(i, j - 1) && (self->moveType & TOP) && speedway.nbpl[j-1] < speedway.length-1 && speedway.road[next_meter][j] == -1)
                moved = self->try_move(self, j - 1);
            if (!moved)
                moved = self->try_move(self, j);
            if (!moved && speedway.exists(i, j + 1) && speedway.nbpl[j+1] < speedway.length-1 && ((mem = speedway.road[i][j+1]) == -1 || (mem != -1 && bikers[mem]->moved)))
                moved = self->try_move(self, j + 1);
            self->moved = true;
        }
        //else
        //    printf("___%sBiker %d%s___ cant't move!\n", self->color, self->id, RESET);

        for (size_t i = 0; i < 4; i++) {
            //printf("%sBiker %d%s unlocked %s\n", self->color, self->id, RESET, getsymbol(i));
            V(&(self->mtxs[i]));
        }
        par++;
        self->totalTime += 20;
        self->moveType = TOPDOWN;
        if (moved && self->i == 0) { // Trigger events each completed lap
            if (self->lap != -1) {
                sb->add_score(sb, self);
                self->calc_new_speed(self);
            }
            par = 1;
            (self->lap)++;
            if ((self->lap+1)%15 == 0 && event(0.01) && sb->tot_num_bikers > 5 && (self->lap+1) != 0) { // Break it down?
                P(&(speedway.mtxs[self->i][self->j]));
                speedway.road[self->i][self->j] = -1;
                V(&(speedway.mtxs[self->i][self->j]));
                biker_status = BROKEN;
                P(&(speedway.mymtx));
                speedway.nbpl[self->j]--;
                V(&(speedway.mymtx));
                P(&(sb->scbr_mtx));
                sb->act_num_bikers--;
                sb->tot_num_bikers--;
                V(&(sb->scbr_mtx));
            }
            if (self->lap == speedway.laps) {
                P(&(speedway.mtxs[self->i][self->j]));
                speedway.road[self->i][self->j] = -1;
                V(&(speedway.mtxs[self->i][self->j]));
                biker_status = FINISHED;
                P(&(speedway.mymtx));
                speedway.nbpl[self->j]--;
                V(&(speedway.mymtx));
                P(&(sb->scbr_mtx));
                sb->act_num_bikers--;
                V(&(sb->scbr_mtx));
            }
        }

        //printf("ESPERANDOOOOOOOOOOO 1 %d\n", self->id);
        // Wait all other bikers move
        pthread_barrier_wait(&barr);
        if(biker_status == BROKEN){
            break_biker(self);
            break;
        }
        else if(biker_status == FINISHED) {
            dummy_threads->run_next(dummy_threads);
            break;
        }
        for (int k = 0; k < 4; k++) {
            if (!(self->used_mtx[k]))
                P(&(self->mtxs[k]));
            self->used_mtx[k] = false;
        }
        self->moved = (par%self->speed == 0)? false : true;

        i = self->i; // The current meter
        j = self->j; // The current lane
        next_meter = (i + 1)%speedway.length;

        P(&(speedway.mymtx));
        if (speedway.exists(i, j-1) && (mem = speedway.road[i][j-1]) != -1)
            add_edge(speedway.g, self->id, mem);
        for (int k = -1; k < 2; k++) {
            if (speedway.exists(next_meter, j+k) && (mem = speedway.road[next_meter][j+k]) != -1)
                add_edge(speedway.g, self->id, mem);
        }
        V(&(speedway.mymtx));
        //printf("ESPERANDOOOOOOOOOOO 2 %d\n", self->id);
        pthread_barrier_wait(&debugger_barr);
        pthread_barrier_wait(&prep_barr);
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
<<<<<<< HEAD
    //TODO: UPDATE THE LSP IN SOME PLACE
    printf("Ciclista %u (%uº lugar na classificação) quebrou na volta %u\n", self->id, self->lsp + 1, self->lap + 1);
=======
    u_int pos = 0;
    Buffer b = new_buffer(-1, speedway.num_bikers);
    for (int i = 0; i < speedway.num_bikers; i++)
        b->append(b, bikers[i]->id, bikers[i]->score, -1);
    qsort(b->data, b->i, sizeof(struct score_s), &compareTo);
    while (b->data[pos].id != self->id) pos++;
    printf("Ciclista %u (%uº lugar na classificação) quebrou na volta %u\n", self->id, pos + 1, self->lap + 1);
>>>>>>> 1bae2ae93fd9062c0b1f93ab12a74bdeb8b53c4a
    self->broken = true;
    P(&broken_mtx);
        broken->append(broken, self->id, self->score, self->totalTime);
        dummy_threads->run_next(dummy_threads);
    V(&broken_mtx);
    destroy_buffer(b);
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
    //printf("%d Lock %d %d (Up)\n", self->id, next_meter, next_lane);
    // Lock the mutex of the next position
    P(&(speedway.mtxs[next_meter][next_lane]));
    // If the next position in the road is free
    if (speedway.road[next_meter][next_lane] == -1) {
        //printf("%d Lock %d %d (Self)\n", self->id, i, j);
        // Lock the mutex of the current position
        P(&(speedway.mtxs[i][j]));
        //printf("id = %02d, speed = %s , next_meter = %02d, curr_lane = %02d, next_lane = %02d, %s\uf206%s\n", self->id, getspeed(self), next_meter, j, next_lane, self->color, RESET);
        speedway.road[next_meter][next_lane] = self->id;
        speedway.road[i][j] = -1;
        if (j != next_lane) {
            P(&(speedway.mymtx));
            speedway.nbpl[j]--;
            speedway.nbpl[next_lane]++;
            V(&(speedway.mymtx));
        }
        self->i = next_meter;
        self->j = next_lane;
        self->moved = true;
        //printf("%d Unlock %d %d (Self)\n", self->id, i, j);
        V(&(speedway.mtxs[i][j]));
    }
    //printf("%d Unlock %d %d (Up)\n", self->id, next_meter, next_lane);
    V(&(speedway.mtxs[next_meter][next_lane]));

    return self->moved;
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
    if (self->fast && self->lap + 1 >= speedway.laps - 2)
        self->speed = 2;
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
            free(b->used_mtx);
            free(b->color);
            free(b);
        }
    }
    free(bikers);
}

void destroy_all() {
    for (size_t i = 0; i < dummy_threads->i; i++)
        pthread_cancel(dummy_threads->dummyT[i]);
}
