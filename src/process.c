#include "process.h"
#include "memory.h"
#include "scheduler.h"
#include "serial.h"

PCB process_table[MAX_PROCESSES];
int current_process_index = -1;
int process_count = 0;

void exit_process()
{
    process_table[current_process_index].state = TERMINATED;

    serial_puts("Process Terminated. Free the stack memory...\n");
    k_free(process_table[current_process_index].stack_start);
    schedule();
}

void init_process_manager()
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        process_table[i].state = TERMINATED;
        process_table[i].pid = 0;
    }
}

int create_process(void (*entry_point)())
{
    if (process_count >= MAX_PROCESSES)
    {
        return -1;
    }

    int idx = -1;
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].state == TERMINATED)
        {
            idx = i;
            break;
        }
    }
    if (idx == -1)
    {
        return -1;
    }

    void *stack = k_malloc(STACK_SIZE);

    uint32_t *sp = (uint32_t *)((uint8_t *)stack + STACK_SIZE);

    *(--sp) = (uint32_t)exit_process;
    *(--sp) = (uint32_t)entry_point;

    *(--sp) = 0x200;

    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;

    process_table[idx].pid = idx + 1;
    process_table[idx].state = READY;
    process_table[idx].esp = sp;
    process_table[idx].stack_start = stack;

    process_count++;
    return idx;
}
