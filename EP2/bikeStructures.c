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
#include "biker.h"
#include "debugger.h"
#include "randomizer.h"
#include "graph.h"


/*
 * Function: reallocate_scoreboard
 * --------------------------------------------------------
 * Auxiliar function to reallocate the internal buffer array,
 * remapping every old buffer to a new position, then returning
 * the position of doubling the size.
 *
 * @args x  : The biker to insert the array
 *
 * @return  the new position for the buffer
 */
u_int reallocate_scoreboard(Biker x) {
    u_int new_sz = sb.n * 2;
    Buffer *temp = emalloc(new_sz*sizeof(Buffer));
    for (size_t i = 0; i < new_sz; temp[i] = NULL, i++);
    for (size_t i = 0; i < sb.n; i++) {
        if (sb.scores[i] != NULL) {
            u_int new_pos = sb.scores[i]->lap % new_sz;
            temp[new_pos] = sb.scores[i];
        }
    }
    free(sb.scores);
    sb.scores = temp;
    sb.n = new_sz;

    return x->lap % sb.n;
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
        printf("%luº - Ciclista %u\n", i+1, b->data[i].id);
    for (size_t i = 0; i < broken->i; i++) {
        Biker x = bikers[broken->data[i].id];
        if (x->lap <= b->lap)
            printf("Quebrou na volta %u - Ciclista %u\n", x->lap + 1, x->id);
    }
    if ((b->lap + 1)%10 == 0) {
        printf("Pontuação na volta %d\n", b->lap + 1);
        qsort(b->data, b->i, sizeof(struct score_s), &compareTo);
        for (size_t i = 0; i < b->i; i++)
            printf("%luº - Ciclista %u - %upts\n", i+1, b->data[i].id, b->data[i].score);
        for (size_t i = 0; i < broken->i; i++) {
            Biker x = bikers[broken->data[i].id];
            if (x->lap <= b->lap)
                printf("Quebrou na volta %u - Ciclista %u - %upts\n", x->lap + 1, x->id, x->score);
        }
    }
    if (b->lap == speedway.laps - 1) {
        qsort(b->data, b->i, sizeof(struct score_s), &compareTo);
        qsort(broken->data, broken->i, sizeof(struct score_s), &compareTo);
        printf("==== Relatório Final ====\n");
        for (size_t i = 0; i < b->i; i++)
            printf("%luº - Ciclista %u - %gs - %upts\n", i+1, b->data[i].id, ((float)b->data[i].time)/1000.0, b->data[i].score);
        for (size_t i = 0; i < broken->i; i++) {
            Biker x = bikers[broken->data[i].id];
            printf("Quebrou na volta %u - Ciclista %u - %gs - %upts\n", x->lap + 1, x->id, ((float)x->totalTime)/1000.0, x->score);
        }
    }
}

/*
 * Function: add_score
 * --------------------------------------------------------
 * Add a new score to the Scoreboard, sending the new biker
 *
 * @args x  : the biker
 *
 * @return
 */
void add_score(Biker x) {
    P(&(sb.scbr_mtx));
    u_int pos = x->lap % sb.n;
    u_int interval = (x->lap + 1)/15;
    if (sb.scores[pos] && sb.scores[pos]->lap != x->lap)
        pos = reallocate_scoreboard(x);
    if (sb.scores[pos] == NULL) {
        sb.scores[pos] = new_buffer(x->lap, sb.tot_num_bikers[interval]);
        printf("%d %d\n", interval, sb.tot_num_bikers[interval]);
    }
    Buffer prev_b = sb.scores[(pos - 1 + sb.n)%sb.n];
    Buffer b = sb.scores[pos];

    P(&(b->mtx));
    if (prev_b != NULL && prev_b->lap + 1 == b->lap && prev_b->i == 1)
        x->score += 20;
    if ((b->lap+1)%10 == 0 && (b->lap + 1) != 0 && b->i <= 4) {
        if (b->i == 0) x->score += 5;
        else if (b->i == 1) x->score += 3;
        else if (b->i == 2) x->score += 2;
        else if (b->i == 3) x->score += 1;
    }
    b->append(b, x->id, x->score, x->totalTime);
    if (b->lap + 1 == speedway.laps - 2 && !sb.foundFast && b->i == 1 && event(0.1)) {
        Biker x = bikers[randint(0, speedway.num_bikers)];
        while (x->broken)
            x = bikers[randint(0, speedway.num_bikers)];
        x->fast = true;
        sb.foundFast = true;
    }
    V(&(b->mtx));

    if(sb.scores[pos] != NULL && b->i == sb.tot_num_bikers[interval]) {
        printf("%d == %d %d\n", b->i, interval, sb.tot_num_bikers[interval]);
        print_buffer(b);
        destroy_buffer(sb.scores[pos]);
        sb.scores[pos] = NULL;
    }
    V(&(sb.scbr_mtx));
}

/*
 * Function: append
 * --------------------------------------------------------
 * Function to add a new id and score to the buffer_s
 *
 * @args b : the buffer
 *       id: the id of the biker to add
 *       score : the score of the biker to add
 *
 * @return
 */
void append(Buffer b, u_int id, u_int score, u_lint t) {
    b->data[b->i].id = id;
    b->data[b->i].score = score;
    b->data[b->i].time = t;
    b->i++;
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
    return (i >= 0 && i < speedway.length && j >= 0 && j < NUM_LANES);
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
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while (sb.act_num_bikers != 0) {
        pthread_barrier_wait(&debugger_barr);
        pthread_barrier_wait(&prep_barr);
        pthread_barrier_wait(&barr);
    }
    return NULL;
}

/*
 * Function: run_next
 * --------------------------------------------------------
 * Run the next available dummy thread in the dt struct
 *
 * @args dt : The DummyThreads structure
 *
 * @return
 */
void run_next(DummyThreads dt) {
    if(dt->i == dt->size)
        die("All the threads were ran!");
    P(&(dt->dummy_mtx));
    pthread_create(&(dt->dummyT[dt->i]), NULL, dt->dummy_func, NULL);
    dt->i++;
    //print_dummy();
    V(&(dt->dummy_mtx));
}

void create_speedway(u_int d, u_int laps, u_int num_bikers) {
    speedway.road = emalloc(d*sizeof(u_int*));
    speedway.length = d;
    speedway.laps = laps;
    speedway.num_bikers = num_bikers;
    for (int i = 0; i < d; i++) {
        speedway.road[i] = emalloc(NUM_LANES*sizeof(u_int));
        for(int j = 0; j < NUM_LANES; j++)
            speedway.road[i][j] = -1; // infinity :'(
    }
    speedway.mtxs = emalloc(d*sizeof(pthread_mutex_t*));
    for (int i = 0; i < d; i++) {
        speedway.mtxs[i] = emalloc(NUM_LANES*sizeof(pthread_mutex_t));
        for(int j = 0; j < NUM_LANES; j++)
            pthread_mutex_init(&(speedway.mtxs[i][j]), NULL);
    }
    pthread_mutex_init(&(speedway.mymtx), NULL);
    speedway.nbpl = emalloc(NUM_LANES*sizeof(u_int));
    for (int i = 0; i < NUM_LANES; i++)
        speedway.nbpl[i] = 0;
    speedway.moveTypes = emalloc(NUM_LANES*sizeof(Move));
    speedway.moveTypes[0] = DOWN;
    for (int i = 1; i < NUM_LANES-1; i++)
        speedway.moveTypes[i] = TOPDOWN;
    speedway.moveTypes[NUM_LANES-1] = TOP;
    speedway.g = new_grafinho(num_bikers);
    speedway.sccl = new_Stacklist(speedway.length - 1);
    speedway.exists = &exists;
}

void destroy_speedway() {
    for (int i = 0; i < speedway.length; i++) {
        free(speedway.road[i]);
        free(speedway.mtxs[i]);
    }
    free(speedway.nbpl);
    free(speedway.moveTypes);
    free(speedway.road);
    free(speedway.mtxs);
    destroy_Stacklist(speedway.sccl);
    destroy_grafinho(speedway.g);
}

void new_scoreboard(u_int laps, u_int num_bikers) {
    u_int init_sz = 2 + (laps - 1)/4;
    sb.scores = emalloc(init_sz*sizeof(Buffer));
    for (size_t i = 0; i < init_sz; sb.scores[i++] = NULL);
    sb.n = init_sz;
    sb.foundFast = false;
    sb.tot_num_bikers = emalloc((num_bikers/15 + 2)*sizeof(u_int));
    for (int i = 0; i <= (num_bikers/15 + 1); sb.tot_num_bikers[i] = num_bikers, i++)
    sb.act_num_bikers = num_bikers;
    pthread_mutex_init(&(sb.scbr_mtx), NULL);
    sb.add_score = &add_score;
}

void destroy_scoreboard() {
    for (int i = 0; i < sb.n; i++) {
        if (sb.scores[i] != NULL)
            destroy_buffer(sb.scores[i]);
    }
    free(sb.scores);
    free(sb.tot_num_bikers);
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

void create_dummy_threads(u_int numBikers) {
    dummy_threads = emalloc(sizeof(struct dummy_s));
    dummy_threads->i = 0;
    dummy_threads->size = numBikers;
    pthread_mutex_init(&(dummy_threads->dummy_mtx), NULL);
    dummy_threads->dummyT = emalloc(numBikers*sizeof(pthread_t));
    dummy_threads->dummy_func = &dummy;
    dummy_threads->run_next = &run_next;
}

void destroy_dummy_threads(){
    pthread_mutex_destroy(&(dummy_threads->dummy_mtx));
    free(dummy_threads->dummyT);
    free(dummy_threads);
}
