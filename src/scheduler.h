#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

void init_scheduler();

void schedule();

extern void context_switch(uint32_t** old_sp, uint32_t* new_sp);

#endif