#include <string.h>
#include "debugger.h"
#include "error.h"
void debug_road() {
    Road spdw = *(&speedway);

    u_int lim = spdw.lanes + spdw.lanes + 2;
    for (int i = 0; i < lim - 1; i++) {
        char *l = emalloc((spdw.length + 1)*sizeof(char));
        l[spdw.length] = 0;
        if(i == 0){
            l[0] = '#';
            l[spdw.length - 1] = '#';
            for (int j = 1; j < spdw.length - 1; l[j++]='-');
        }
        else {
            if(i != lim - 2){
                l[0] = '|';
                l[spdw.length - 1] = '|';
            }
            else{
                l[0] = '#';
                l[spdw.length - 1] = '#';
            }
            if(i%2 != 0){
                for (int j = 1; j < spdw.length - 1; j++){
                    if(spdw.road[j][i/2] != -1)
                        l[j] = 'X'; // Get info from biker and colorize it
                    else
                        l[j] = ' ';
                }

            }
            else
                for (int j = 1; j < spdw.length - 1; l[j++]='_');
        }
        printf("%s\n", l);
    }
}
