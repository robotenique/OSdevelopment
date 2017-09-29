#include "error.h"
#include "bikeStructures.h"

Scoreboard new_scoreboard(u_int laps, u_int num_bikers) {
    u_int init_sz = 2 + (laps - 1)/4 ;
    Scoreboard sb = emalloc(sizeof(struct scbr_s*));
    sb->scores = emalloc(init_sz*sizeof(Buffer));
    sb->n = init_sz;
    sb->num_bikers = num_bikers;
    return sb;
}

void add_info(Buffer b, Biker x) {
    int str_sz = snprintf(NULL, 0, "Pos(%u) - Biker %u", x->id, x->lap);
    if(str_sz < 0)
        die("Can't store biker info to scoreboard.");
    char *str_tmp = emalloc((str_sz + 1)*sizeof(char));
    snprintf(str_tmp, str_sz + 1, "Pos(%u) - Biker %u", x->id, x->lap);
    b->append(b, str_tmp);
    free(str_tmp);
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

    // Lock Mutex
    if(sb->scores[pos] == NULL) sb->scores[pos] = new_buffer(x->lap);
    else if(sb->scores[pos]->lap != x->lap)
        pos = reallocate_scoreboard(sb, x);
    add_info(sb->scores[pos], x);
    if(sb->scores[pos]->i == sb->num_bikers)
        printf("meh\n"); // TODO: Delete this buffer and print everything...
    // Unlock Mutex
}

void create_speedway(u_int d){
    speedway.road = emalloc(d*sizeof(u_int*));
    speedway.length = d;
    speedway.lanes = NUM_LANES;
    for (int i = 0; i < d; i++)
        speedway.road[i] = emalloc(10*sizeof(u_int));
    for (int i = 0; i < d; i++)
        for(int j = 0; j < 10; j++)
            speedway.road[i][j] = -1; // infinity :'(
}

Buffer new_buffer(u_int lap) {
    u_int init_sz = 10;
    Buffer b = emalloc(sizeof(struct buffer_s*));
    b->lap = lap;
    b->i = 0;
    b->data = emalloc(init_sz*sizeof(char*));
    b->size = init_sz;
    b->append = &append;
    return b;
    // TODO: initialize the mutex
}

void destroy_buffer(Buffer b){
    for (size_t i = 0; i < b->i; i++)
        free(b->data[i]);
    free(b->data);
    free(b);
}

void append(Buffer b, char *s) {
    b->data[b->i] = estrdup(s);
    b->i++;
    if(b->i == b->size) {
        printf("Realocando Buffer...\n");
        char **temp = emalloc((b->size)*2*sizeof(char*));
        for (size_t i = 0; i < b->size; i++)
            temp[i] = b->data[i];
        free(b->data);
        b->data = temp;
        b->size = (b->size)*2;
    }
}
