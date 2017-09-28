#include <string.h>
#include "debugger.h"
#include "error.h"
#include "randomizer.h"
#include <stdio.h>

#define RESET "\x1b[0m"

const char* colors[] = {"\x1b[31m",
                        "\x1b[32m",
                        "\x1b[33m",
                        "\x1b[34m",
                        "\x1b[35m",
                        "\x1b[36m",
                        "\x1b[38;5;22m",
                        "\x1b[38;5;23m",
                        "\x1b[38;5;24m",
                        "\x1b[38;5;25m",
                        "\x1b[38;5;26m",
                        "\x1b[38;5;27m",
                        "\x1b[38;5;28m",
                        "\x1b[38;5;29m",
                        "\x1b[38;5;30m"};


void random_initialize(u_int nBikers) {
    Road spdw = speedway;
    bikers = emalloc(nBikers*sizeof(Biker));
    for (size_t jj = 0; jj < nBikers; jj++) {
        bikers[jj] = emalloc(sizeof(struct biker));
    }
    for (int i = 0; i < nBikers; i++) {
        Biker b = bikers[i];
        b->lap = 0;
        b->id = i;
        b->score = 0;
        b->speed = 0;
        b->color = i;
        u_int meter = 0;
        u_int lane = 0;
        while(spdw.road[(meter = randint(0, spdw.length))][(lane = randint(0, spdw.lanes))] != -1);
        spdw.road[meter][lane] = i;
    }
    spdw.road[6][5] = 3;
    spdw.road[5][5] = 5;



}

void debug_road() {
    Road spdw = *(&speedway);
    bool biker_lane;
    u_int lastbk = 0;
    printf("+");
    for (size_t i = 0; i < spdw.length; printf("-"), i++);
    printf("+\n");
    for (size_t i = 0; i < spdw.lanes; i++) {
        printf("|");
        for (size_t j = 0; j < spdw.length; j++) {
            u_int id;
            if((id = spdw.road[j][i]) != -1){
                printf("%s\uf206%s", colors[id], RESET);
            }
            else
                printf(" ");
        }
        printf("|\n+");
        for (size_t j = 0; j < spdw.length;printf("-"), j++);
        printf("+\n");
    }
}
