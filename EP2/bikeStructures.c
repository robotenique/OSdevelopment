#include <pthread.h>
#include "error.h"
#include "bikeStructures.h"
#include "debugger.h"

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
    Scoreboard sb = emalloc(sizeof(struct scbr_s*));
    sb->scores = emalloc(init_sz*sizeof(Buffer));
    sb->n = init_sz;
    sb->num_bikers = num_bikers;
    return sb;
}

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

void add_score(Scoreboard sb, Biker x) {
    /* TODO: when a player is the last one to add, it
        needs to print out the information of everything..*/
    u_int pos = x->lap % sb->n;
    // TODO: LOCK A MUTEX of this position... maybe it's
    // better to keep the mtx list in the Scoreboard struct...

    if(sb->scores[pos] && sb->scores[pos]->lap != x->lap)
        pos = reallocate_scoreboard(sb, x);
    if(sb->scores[pos] == NULL)
        sb->scores[pos] = new_buffer(x->lap, sb->num_bikers);
    add_info(sb->scores[pos], x);
    printf("%s\n", sb->scores[pos]->data[0]);
    debug_buffer(sb->scores[pos]);
    x->lap++;
    if(sb->scores[pos]->i == sb->num_bikers) {
        printf("meh\n"); // TODO: Delete this buffer and print everything...
        debug_buffer(sb->scores[pos]);
        destroy_buffer(sb->scores[pos]);
        sb->scores[pos] = NULL;
    }
}

void add_info(Buffer b, Biker x) {
    printf("Got to add_info\n");
    int str_sz = snprintf(NULL, 0, "Pos(%u) - Biker %u", x->i, x->id);
    if(str_sz < 0)
        die("Can't store biker info to scoreboard.");
    pthread_mutex_lock(&(b->mtx));
    b->data[b->i] = emalloc((str_sz + 1)*sizeof(char));
    snprintf(b->data[b->i], str_sz + 1, "Pos(%u) - Biker %u", x->i, x->id);
    printf("%s\n", b->data[0]);
    b->i++;
    pthread_mutex_unlock(&(b->mtx));
    /*char *str_tmp = emalloc((str_sz + 1)*sizeof(char));
    snprintf(str_tmp, str_sz + 1, "Pos(%u) - Biker %u", x->i, x->id);
    b->append(b, str_tmp);
    free(str_tmp);
    printf("Out of add_info\n");*/
}

void destroy_scoreboard(Scoreboard sb) {
    /*for (int i = 0; i < sb->n; i++) {
        // This can be buggy!!!!
        if (sb->scores[i] != NULL)
            free(sb->scores[i]);
    }*/
    free(sb->scores);
    free(sb);
}

Buffer new_buffer(u_int lap, u_int num_bikers) {
    Buffer b = emalloc(sizeof(struct buffer_s*));
    b->lap = lap;
    b->i = 0;
    b->data = emalloc(num_bikers*sizeof(char*));
    b->size = num_bikers;
    b->append = &append;
    pthread_mutex_init(&(b->mtx), NULL);
    return b;
}

void append(Buffer b, char *s) {
    printf("Got to append\n");
    pthread_mutex_lock(&(b->mtx));
    b->data[b->i] = s;
    b->i++;
    printf("Out of append\n");
    pthread_mutex_unlock(&(b->mtx));
}

void destroy_buffer(Buffer b){
    for (size_t i = 0; i < b->i; i++)
        free(b->data[i]);
    free(b->data);
    free(b);
}

int exists(int i, int j) {
    return (i >= 0 && i < speedway.length && j >= 0 && j < speedway.lanes);
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
            printf("%d Lock %d %d (Same up)\n", self->id, i, j+1);
            pthread_mutex_lock(&(speedway.mtxs[i][j+1]));
            if (speedway.road[i][j+1] != -1)
                up = true;
            printf("%d Unlock %d %d (Same up)\n", self->id, i, j+1);
            pthread_mutex_unlock(&(speedway.mtxs[i][j+1]));
        }
        if (!up) {
            u_int im = (i+1)%speedway.length;
            if (exists(im, j-1)) {
                printf("%d Lock %d %d (Down)\n", self->id, im, j-1);
                pthread_mutex_lock(&(speedway.mtxs[im][j-1]));
                if (speedway.road[im][j-1] == -1) {
                    printf("%d Lock %d %d (Self)\n", self->id, i, j);
                    pthread_mutex_lock(&(speedway.mtxs[i][j]));
                    printf("Down %d %d %d %s\uf206%s\n", self->id, im, j, self->color, RESET);
                    speedway.road[im][j-1] = self->id;
                    speedway.road[i][j] = -1;
                    self->i = im;
                    self->j = j-1;
                    moved = true;
                    printf("%d Unlock %d %d (Self)\n", self->id, i, j);
                    pthread_mutex_unlock(&(speedway.mtxs[i][j]));
                }
                printf("%d Unlock %d %d (Down)\n", self->id, im, j-1);
                pthread_mutex_unlock(&(speedway.mtxs[im][j-1]));
            }
            if (!moved && exists(im, j)) {
                printf("%d Lock %d %d (Mid)\n", self->id, im, j);
                pthread_mutex_lock(&(speedway.mtxs[im][j]));
                if (speedway.road[im][j] == -1) {
                    printf("%d Lock %d %d (Self)\n", self->id, i, j);
                    pthread_mutex_lock(&(speedway.mtxs[i][j]));
                    printf("Mid %d %d %d %s\uf206%s\n", self->id, im, j, self->color, RESET);
                    speedway.road[im][j] = self->id;
                    speedway.road[i][j] = -1;
                    self->i = im;
                    self->j = j;
                    moved = true;
                    printf("%d Unlock %d %d (Self)\n", self->id, i, j);
                    pthread_mutex_unlock(&(speedway.mtxs[i][j]));
                }
                printf("%d Unlock %d %d (Mid)\n", self->id, im, j);
                pthread_mutex_unlock(&(speedway.mtxs[im][j]));
            }
            if (!moved && exists(im, j+1)) {
                printf("%d Lock %d %d (Up)\n", self->id, im, j+1);
                pthread_mutex_lock(&(speedway.mtxs[im][j+1]));
                if (speedway.road[im][j+1] == -1) {
                    printf("%d Lock %d %d (Self)\n", self->id, i, j);
                    pthread_mutex_lock(&(speedway.mtxs[i][j]));
                    printf("Up %d %d %d %s\uf206%s\n", self->id, im, j, self->color, RESET);
                    speedway.road[im][j+1] = self->id;
                    speedway.road[i][j] = -1;
                    self->i = im;
                    self->j = j+1;
                    moved = true;
                    printf("%d Unlock %d %d (Self)\n", self->id, i, j);
                    pthread_mutex_unlock(&(speedway.mtxs[i][j]));
                }
                printf("%d Unlock %d %d (Up)\n", self->id, im, j+1);
                pthread_mutex_unlock(&(speedway.mtxs[im][j+1]));
            }
        }
        if (!moved)
            printf("Still %d %d %d %s\uf206%s\n", self->id, i, j, self->color, RESET);
        printf("%d waiting...\n", self->id);
        pthread_barrier_wait(&barr);
        if (moved && self->i == 0)
            add_score(sb, self);
        printf("%d waiting 2...\n", self->id);
        pthread_barrier_wait(&barr2);
    }
    return NULL;
}
