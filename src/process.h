#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

#define MAX_PROCESSES 8
#define STACK_SIZE 4096 

typedef enum {
    READY,
    CURRENT,
    TERMINATED
} ProcessState;

typedef struct {
    uint32_t* esp; 
    uint32_t pid;   
    ProcessState state;
    void* stack_start;
} PCB;

extern PCB process_table[MAX_PROCESSES];
extern int current_process_index;

// Function Prototypes
void init_process_manager();
int create_process(void (*entry_point)());
void exit_process();
#endif