#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

#include "bikeStructures.h"

#define RESET "\x1b[0m"

void debug_road();
void random_initialize(u_int);
void debug_buffer(Buffer);

#endif
