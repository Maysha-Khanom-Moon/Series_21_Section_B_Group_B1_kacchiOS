#include "process.h"
#include "memory.h"
#include "scheduler.h"
#include "serial.h"

PCB process_table[MAX_PROCESSES];
int current_process_index = -1;
int process_count = 0;

static void print_dec(int num)
{

    if (num == 0)
    {
        serial_putc('0');
        return;
    }

    char buf[16];
    int i = 0;

    while (num > 0)
    {

        buf[i++] = (num % 10) + '0';
        num /= 10;
    }

    while (i > 0)

        serial_putc(buf[--i]);
}

static void print_hex(uint32_t num)
{

    serial_puts("0x");
    char hex[] = "0123456789ABCDEF";
    for (int i = 28; i >= 0; i -= 4)
    {
        serial_putc(hex[(num >> i) & 0xF]);
    }
}

void exit_process()
{
    serial_puts("[Process Manager] PID ");
    print_dec(process_table[current_process_index].pid);
    serial_puts(" Terminated. (State: TERMINATED)\n");
    process_table[current_process_index].state = TERMINATED;
    serial_puts("   -> Freeing Stack Memory...\n");
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
        return -1;

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
        return -1;

    void *stack = k_malloc(STACK_SIZE);

    serial_puts("[Process Manager] Creating PID ");
    print_dec(idx + 1);

    serial_puts("...\n");
    serial_puts("   -> State: READY\n");
    serial_puts("   -> Stack Allocated: ");
    print_dec(STACK_SIZE);

    serial_puts(" bytes at ");
    print_hex((uint32_t)stack);
    serial_puts("\n");

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
    *(--sp) = 0;

    process_table[idx].pid = idx + 1;
    process_table[idx].state = READY;
    process_table[idx].esp = sp;
    process_table[idx].stack_start = stack;
    process_count++;

    return idx;
}