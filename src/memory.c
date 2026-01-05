#include "memory.h"
extern uint8_t __kernel_end; 
static uint8_t* heap_start = &__kernel_end;
static uint8_t* heap_top = &__kernel_end;
typedef struct BlockHeader {
    uint32_t size;            
    uint8_t is_free;          
    struct BlockHeader* next; 
} BlockHeader;

static BlockHeader* free_list_head = 0; 

void* k_malloc(uint32_t size) 
{
    BlockHeader* curr = free_list_head;

    while (curr) 
    {
        if (curr->is_free && curr->size >= size) 
        {
            
            curr->is_free = 0; 
           
            return (void*)(curr + 1); 
        }
        curr = curr->next;
    }
    uint32_t total_size = sizeof(BlockHeader) + size;
    
    BlockHeader* new_block = (BlockHeader*)heap_top;
    
    heap_top += total_size;

    new_block->size = size;
    new_block->is_free = 0; 
    new_block->next = 0;   

    if (free_list_head == 0) 
    {
        
        free_list_head = new_block;
    } 
    else 
    {
        
        BlockHeader* temp = free_list_head;
        while (temp->next != 0) 
        {
            temp = temp->next;
        }
        temp->next = new_block;
    }


    return (void*)(new_block + 1);
}



void k_free(void* ptr) 
{
    if (ptr == 0) return;

   
    BlockHeader* header = (BlockHeader*)ptr - 1;

   
    header->is_free = 1;
    if (header->next != 0 && header->next->is_free == 1) 
    {
        
        header->size += sizeof(BlockHeader) + header->next->size;
        header->next = header->next->next;
        
    }
}
