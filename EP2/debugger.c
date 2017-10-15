/*
 * @author: João Gabriel Basi Nº USP: 9793801
 * @author: Juliano Garcia de Oliveira Nº USP: 9277086
 *
 * MAC0422
 * 16/10/17
 *
 * File for the debugging functions
 */
#include <string.h>
#include <stdio.h>
#include "debugger.h"
#include "error.h"
#include "randomizer.h"
#include "biker.h"
#include "math.h"

const char* colors[] = {"\x1b[31m", "\x1b[32m", "\x1b[33m", "\x1b[34m", "\x1b[35m", "\x1b[36m", "\x1b[38;5;22m", "\x1b[38;5;23m", "\x1b[38;5;24m", "\x1b[38;5;25m", "\x1b[38;5;26m", "\x1b[38;5;27m", "\x1b[38;5;28m", "\x1b[38;5;29m", "\x1b[38;5;30m", "\x1b[38;5;31m", "\x1b[38;5;32m", "\x1b[38;5;33m", "\x1b[38;5;34m", "\x1b[38;5;35m", "\x1b[38;5;36m", "\x1b[38;5;37m", "\x1b[38;5;38m", "\x1b[38;5;39m", "\x1b[38;5;40m", "\x1b[38;5;41m", "\x1b[38;5;42m", "\x1b[38;5;43m", "\x1b[38;5;44m", "\x1b[38;5;45m", "\x1b[38;5;46m", "\x1b[38;5;47m", "\x1b[38;5;48m", "\x1b[38;5;49m", "\x1b[38;5;50m", "\x1b[38;5;51m", "\x1b[38;5;52m", "\x1b[38;5;53m", "\x1b[38;5;54m", "\x1b[38;5;55m", "\x1b[38;5;56m", "\x1b[38;5;57m", "\x1b[38;5;58m", "\x1b[38;5;59m", "\x1b[38;5;60m", "\x1b[38;5;61m", "\x1b[38;5;62m", "\x1b[38;5;63m", "\x1b[38;5;64m", "\x1b[38;5;65m", "\x1b[38;5;66m", "\x1b[38;5;67m", "\x1b[38;5;68m", "\x1b[38;5;69m", "\x1b[38;5;70m", "\x1b[38;5;71m", "\x1b[38;5;72m", "\x1b[38;5;73m", "\x1b[38;5;74m", "\x1b[38;5;75m", "\x1b[38;5;76m", "\x1b[38;5;77m", "\x1b[38;5;78m", "\x1b[38;5;79m", "\x1b[38;5;80m", "\x1b[38;5;81m", "\x1b[38;5;82m", "\x1b[38;5;83m", "\x1b[38;5;84m", "\x1b[38;5;85m", "\x1b[38;5;86m", "\x1b[38;5;87m", "\x1b[38;5;88m", "\x1b[38;5;89m", "\x1b[38;5;90m", "\x1b[38;5;91m", "\x1b[38;5;92m", "\x1b[38;5;93m", "\x1b[38;5;94m", "\x1b[38;5;95m", "\x1b[38;5;96m", "\x1b[38;5;97m", "\x1b[38;5;98m", "\x1b[38;5;99m", "\x1b[38;5;100m", "\x1b[38;5;101m", "\x1b[38;5;102m", "\x1b[38;5;103m", "\x1b[38;5;104m", "\x1b[38;5;105m", "\x1b[38;5;106m", "\x1b[38;5;107m", "\x1b[38;5;108m", "\x1b[38;5;109m", "\x1b[38;5;110m", "\x1b[38;5;111m", "\x1b[38;5;112m", "\x1b[38;5;113m", "\x1b[38;5;114m", "\x1b[38;5;115m", "\x1b[38;5;116m", "\x1b[38;5;117m", "\x1b[38;5;118m", "\x1b[38;5;119m", "\x1b[38;5;120m", "\x1b[38;5;121m", "\x1b[38;5;122m", "\x1b[38;5;123m", "\x1b[38;5;124m", "\x1b[38;5;125m", "\x1b[38;5;126m", "\x1b[38;5;127m", "\x1b[38;5;128m", "\x1b[38;5;129m", "\x1b[38;5;130m", "\x1b[38;5;131m", "\x1b[38;5;132m", "\x1b[38;5;133m", "\x1b[38;5;134m", "\x1b[38;5;135m", "\x1b[38;5;136m", "\x1b[38;5;137m", "\x1b[38;5;138m", "\x1b[38;5;139m", "\x1b[38;5;140m", "\x1b[38;5;141m", "\x1b[38;5;142m", "\x1b[38;5;143m", "\x1b[38;5;144m", "\x1b[38;5;145m", "\x1b[38;5;146m", "\x1b[38;5;147m", "\x1b[38;5;148m", "\x1b[38;5;149m", "\x1b[38;5;150m", "\x1b[38;5;151m", "\x1b[38;5;152m", "\x1b[38;5;153m", "\x1b[38;5;154m", "\x1b[38;5;155m", "\x1b[38;5;156m", "\x1b[38;5;157m", "\x1b[38;5;158m", "\x1b[38;5;159m", "\x1b[38;5;160m", "\x1b[38;5;161m", "\x1b[38;5;162m", "\x1b[38;5;163m", "\x1b[38;5;164m", "\x1b[38;5;165m", "\x1b[38;5;166m", "\x1b[38;5;167m", "\x1b[38;5;168m", "\x1b[38;5;169m", "\x1b[38;5;170m", "\x1b[38;5;171m", "\x1b[38;5;172m", "\x1b[38;5;173m", "\x1b[38;5;174m", "\x1b[38;5;175m", "\x1b[38;5;176m", "\x1b[38;5;177m", "\x1b[38;5;178m", "\x1b[38;5;179m", "\x1b[38;5;180m", "\x1b[38;5;181m", "\x1b[38;5;182m", "\x1b[38;5;183m", "\x1b[38;5;184m", "\x1b[38;5;185m", "\x1b[38;5;186m", "\x1b[38;5;187m", "\x1b[38;5;188m", "\x1b[38;5;189m", "\x1b[38;5;190m", "\x1b[38;5;191m", "\x1b[38;5;192m", "\x1b[38;5;193m", "\x1b[38;5;194m", "\x1b[38;5;195m", "\x1b[38;5;196m", "\x1b[38;5;197m", "\x1b[38;5;198m", "\x1b[38;5;199m", "\x1b[38;5;200m", "\x1b[38;5;201m", "\x1b[38;5;202m", "\x1b[38;5;203m", "\x1b[38;5;204m", "\x1b[38;5;205m", "\x1b[38;5;206m", "\x1b[38;5;207m", "\x1b[38;5;208m", "\x1b[38;5;209m", "\x1b[38;5;210m", "\x1b[38;5;211m", "\x1b[38;5;212m", "\x1b[38;5;213m", "\x1b[38;5;214m", "\x1b[38;5;215m", "\x1b[38;5;216m", "\x1b[38;5;217m", "\x1b[38;5;218m", "\x1b[38;5;219m", "\x1b[38;5;220m", "\x1b[38;5;221m", "\x1b[38;5;222m", "\x1b[38;5;223m", "\x1b[38;5;224m", "\x1b[38;5;225m", "\x1b[38;5;226m", "\x1b[38;5;227m", "\x1b[38;5;228m", "\x1b[38;5;229m", "\x1b[38;5;230m", "\x1b[38;5;231m", "\x1b[38;5;232m", "\x1b[38;5;233m", "\x1b[38;5;234m", "\x1b[38;5;235m", "\x1b[38;5;236m", "\x1b[38;5;237m", "\x1b[38;5;238m", "\x1b[38;5;239m", "\x1b[38;5;240m", "\x1b[38;5;241m", "\x1b[38;5;242m", "\x1b[38;5;243m", "\x1b[38;5;244m", "\x1b[38;5;245m", "\x1b[38;5;246m", "\x1b[38;5;247m", "\x1b[38;5;248m", "\x1b[38;5;249m", "\x1b[38;5;250m", "\x1b[38;5;251m", "\x1b[38;5;252m", "\x1b[38;5;253m", "\x1b[38;5;254m"};

void print_centered(u_int num, u_int final_space) {
    u_int numlog = floor(num <= 1 ? 0.0 : log10(num)) + 1;
    double side_spaces = (final_space - numlog)/2.0;
    for (int i = 0; i < ceil(side_spaces); printf(" "), i++);
    printf("%d", num);
    for (int i = 0; i < floor(side_spaces); printf(" "), i++);
}

void debug_road_better() {
    printf("+");
    for (size_t i = 0; i < speedway.length; printf("------"), i++);
    printf("+\n");
    for (size_t i = 0; i < NUM_LANES; i++) {
        printf("|");
        for (size_t j = 0; j < speedway.length; j++) {
            if(speedway.road[j][i] != -1)
                print_centered(speedway.road[j][i], 6);
            else
                printf("  ··  ");
        }
        printf("|\n+");
        for (size_t j = 0; j < speedway.length; printf("------"), j++);
        printf("+\n");
    }
    printf("\n");
}

void debug_road() {
    printf("+");
    for (size_t i = 0; i < speedway.length; printf("-"), i++);
    printf("+\n");
    for (size_t i = 0; i < NUM_LANES; i++) {
        printf("|");
        for (size_t j = 0; j < speedway.length; j++) {
            u_int id;
            if((id = speedway.road[j][i]) != -1)
                if(bikers[id]->fast)
                    printf("%sX%s", bikers[id]->color, RESET);
                else
                    printf("%s0%s", bikers[id]->color, RESET);
            else
                printf("·");
        }
        printf("|\n+");
        for (size_t j = 0; j < speedway.length; printf("-"), j++);
        printf("+\n");
    }
    printf("\n");
}

void debug_buffer(Buffer b) {
    printf("LAP = %u, i = %u, SIZE = %u\n", b->lap, b->i, b->size);
    for (size_t i = 0; i < b->i; i++)
        printf("%luº - Biker %u - %upts\n", i+1, b->data[i].id, b->data[i].score);
}

void print_prog_name() {
    const char *r = colors[randint(1, 230)];
    const char *p = colors[randint(1, 230)];
    printf("\n%s    ____  _ __             _____ %s_                 __      __            \n%s   / __ )(_) /_____       / ___/%s(_)___ ___  __  __/ /___ _/ /_____  _____\n%s  / __  / / //_/ _ \\______\\__ \\/%s / __ `__ \\/ / / / / __ `/ __/ __ \\/ ___/\n%s / /_/ / / ,< /  __/_____/__/ /%s / / / / / / /_/ / / /_/ / /_/ /_/ / /    \n%s/_____/_/_/|_|\\___/     /____/%s_/_/ /_/ /_/\\__,_/_/\\__,_/\\__/\\____/_/%s     \n========================================================================================\n",
    p, r, p, r, p, r, p, r, p, r, RESET);
}

const char *get_color(u_int x) {
    return colors[x];
}
