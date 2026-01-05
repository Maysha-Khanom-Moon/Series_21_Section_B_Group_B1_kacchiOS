#include "scheduler.h"
#include "process.h"
#include "io.h"


static void print_dec(int num) {

    if (num == 0)  { 

        serial_putc('0'); 
        return; 
    }

    char buf[16]; 
    
    int i = 0;
    
    while(num > 0) { 

        buf[i++] = (num % 10) + '0'; 
        
        num /= 10; 
    }
    
    while(i > 0) serial_putc(buf[--i]);
}


void init_scheduler() {

    current_process_index = -1;
}

void schedule() {

    int next_idx = -1;

    int start_idx = (current_process_index == -1) ? 0 : (current_process_index + 1);

    serial_puts("\n[Scheduler] Context Switch:\n");

    
    PCB* prev_proc = 0;
    
    if (current_process_index != -1 && process_table[current_process_index].state == CURRENT) {

        serial_puts("   PID ");
        print_dec(process_table[current_process_index].pid);
        serial_puts(" (CURRENT) -> READY\n");

        process_table[current_process_index].state = READY;
        prev_proc = &process_table[current_process_index];
    }

    else if (current_process_index != -1 && process_table[current_process_index].state == TERMINATED){
       
    }

    for (int i = 0; i < MAX_PROCESSES; i++)  {

        int idx = (start_idx + i) % MAX_PROCESSES;
        
        if (process_table[idx].state == TERMINATED && process_table[idx].pid != 0) {
           
            serial_puts("   PID ");
            print_dec(process_table[idx].pid);
            serial_puts(" (TERMINATED) -> Skipping\n");
        }
        else if (process_table[idx].state == READY) {

            next_idx = idx;
            break;
        }
    }

    if (next_idx == -1) return;

   
    serial_puts("   PID ");
    print_dec(process_table[next_idx].pid);
    serial_puts(" (READY)   -> CURRENT\n");

    current_process_index = next_idx;
    PCB* next_proc = &process_table[next_idx];
    next_proc->state = CURRENT;
    
    uint32_t* dummy_sp;
    uint32_t** old_sp_ptr = (prev_proc) ? &prev_proc->esp : &dummy_sp;

    context_switch(old_sp_ptr, next_proc->esp);
}

__attribute__((naked)) void context_switch(uint32_t** old_sp, uint32_t* new_sp) {
    (void)old_sp; (void)new_sp;
    __asm__ volatile (
        "pusha\n\t"
        "pushf\n\t"
        "mov 40(%esp), %eax\n\t" 
        "mov %esp, (%eax)\n\t"    
        "mov 44(%esp), %esp\n\t"
        "popf\n\t"
        "popa\n\t"
        "ret\n\t"
    );
}