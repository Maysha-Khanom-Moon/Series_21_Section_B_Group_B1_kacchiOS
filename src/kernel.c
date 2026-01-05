#include "types.h"
#include "serial.h"
#include "string.h"
#include "io.h"
#include "process.h"
#include "scheduler.h"

#define MAX_INPUT 128
char global_string[] = "OperatingSystem";
volatile int active_tasks = 0; 
void delay_slow() {
    for (volatile int i = 0; i < 6000000; i++);
}

int get_pid() 
{
    if (current_process_index == -1) return 0;
    return process_table[current_process_index].pid;
}

void print_process_msg(const char* proc_name, const char* msg)
{
    serial_puts("[PID: ");
    int pid = get_pid();
    serial_putc(pid + '0');
    
    serial_puts("] ");
    serial_puts(proc_name);
    serial_puts(": ");
    serial_puts(msg);
    serial_puts("\n");
}
void process_a() 
{
    active_tasks++;
    print_process_msg("Proc A", "Started. String to reverse:");
    serial_puts("          "); 
    serial_puts(global_string);
    serial_puts("\n");

    int len = 0;
    while(global_string[len] != '\0') len++;
    for (int i = 0; i < len / 2; i++) 
    {
        char temp = global_string[i];
        global_string[i] = global_string[len - 1 - i];
        global_string[len - 1 - i] = temp;
      
        if (i == 2) { 
             print_process_msg("Proc A", "Yielding during reverse...");
             schedule(); 
        }
    }

    print_process_msg("Proc A", "Done! Result:");
    serial_puts("          ");
    serial_puts(global_string);
    serial_puts("\n");
    
    active_tasks--;
    exit_process();
}

void process_b() 
{
    active_tasks++;
    print_process_msg("Proc B", "Started. Calculating length...");
    
    delay_slow();

    int len = 0;
    while(global_string[len] != '\0') len++;
    serial_puts("[PID: ");
    serial_putc(get_pid() + '0');
    serial_puts("] Proc B: Length is ");
    
    if (len == 0) serial_putc('0');
    else {
        char buf[10];
        int pos = 0, n = len;
        while(n > 0) { buf[pos++] = (n%10)+'0'; n/=10; }
        while(pos > 0) serial_putc(buf[--pos]);
    }
    serial_puts("\n");

    active_tasks--;
    exit_process();
}
void process_c() 
{
    active_tasks++;
    print_process_msg("Proc C", "Started counting 1 to 50...");

    for (int i = 1; i <= 50; i++) 
    {
        /
        if (i == 100) serial_puts("100");
        else if (i >= 10) {
            serial_putc((i/10) + '0');
            serial_putc((i%10) + '0');
        } else {
            serial_putc(i + '0');
        }
        serial_putc(' ');
        if (i % 10 == 0) 
        {
            serial_puts("\n");
            print_process_msg("Proc C", "Checkpoint reached. Yielding CPU.");
            
            delay_slow();
            schedule();  
            
            print_process_msg("Proc C", "Resumed context!");
        }
    }

    serial_puts("\n");
    print_process_msg("Proc C", "Finished counting.");
    active_tasks--;
    exit_process();
}

void null_process()
{

    char input[MAX_INPUT];
    int pos = 0;
    int shell_initialized = 0;


    while(1)
    {
        if (active_tasks > 0) 
        {
            schedule();
        }
        else 
        {
            
            
            if (shell_initialized == 0) {
               
                serial_puts("\n");
                serial_puts("========================================\n");
                serial_puts("    kacchiOS - Minimal Baremetal OS\n");
                serial_puts("========================================\n");
                serial_puts("All background processes finished.\n");
                serial_puts("Running interactive shell (Null Process)...\n\n");
                shell_initialized = 1;
            }

          
            serial_puts("kacchiOS> ");
            pos = 0;
            
           
            while (1) {
                char c = serial_getc();
                
                
                if (c == '\r' || c == '\n') {
                    input[pos] = '\0';
                    serial_puts("\n");
                    break;
                }
               
                else if ((c == '\b' || c == 0x7F) && pos > 0) {
                    pos--;
                    serial_puts("\b \b");  
                }
               
                else if (c >= 32 && c < 127 && pos < MAX_INPUT - 1) {
                    input[pos++] = c;
                    serial_putc(c);  
                }
            }
            
            
            if (pos > 0) {
                serial_puts("You typed: ");
                serial_puts(input);
                serial_puts("\n");
            }
        }
    }
}

void kmain(void) 
{
    serial_init();
    serial_puts("========================================\n");
    serial_puts("    kacchiOS - Minimal Baremetal OS\n");
    serial_puts("========================================\n");
    serial_puts("Hello from kacchiOS!\n");
    serial_puts("Running null process...\n\n");

    init_process_manager();
    init_scheduler();

    create_process(process_a);
    create_process(process_b);
    create_process(process_c);
    create_process(null_process);

    serial_puts("Starting Scheduler...\n");
    schedule();

    while(1);
}
