#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <execinfo.h>


#include "mem.h"

/*header
#define POOL_SIZE (128 * 1024 * 1024)  // 128MB
#define MIN_SIZE 16                     // 16 bytes minimum
#define MAX_ORDER 23                    // log2(128MB/16) = 23

typedef struct block {
    size_t next:28;
    size_t prev:28;
    size_t order:5;
    size_t is_free:1;
} block_t;

struct arena {
	block_t* free_list[MAX_ORDER + 1];
	size_t total_allocator;
	size_t allocator_count;
	char* memory_pool;
	char cache[2*1024*1024];
};

extern void*(*allocator)(void* ptr, size_t len);
extern struct arena* arena;

end*/

void*(*allocator)(void* ptr, size_t len)=os_alloc;


struct arena* arena=NULL;

struct block* block_next(struct block* in){
	return in->next ? (struct block*)(arena->memory_pool+(in->next-1)*MIN_SIZE) : NULL;
}
struct block* block_prev(struct block* in){
	return in->prev ? (struct block*)(arena->memory_pool+(in->prev-1)*MIN_SIZE) : NULL;
}
int block_no(struct block* in){
	return in ? ((char*)in-arena->memory_pool)/16+1 : 0;
}

// Get the order (power of 2) for a given size
static int get_order(size_t size) {
    if (size <= MIN_SIZE) return 0;
    
    int order = 0;
    size_t block_size = MIN_SIZE;
    
    while (block_size < size) {
        block_size <<= 1;
        order++;
    }
    
    return order;
}

// Get block size for given order
static size_t get_block_size(int order) {
    return MIN_SIZE << order;
}

// Get buddy address
static void* get_buddy(void* block, int order) {
    uintptr_t addr = (uintptr_t)block;
    uintptr_t pool_start = (uintptr_t)arena->memory_pool;
    uintptr_t offset = addr - pool_start;
    size_t block_size = get_block_size(order);
    
    // XOR with block size to get buddy
    uintptr_t buddy_offset = offset ^ block_size;
    return (void*)(pool_start + buddy_offset);
}
// Remove block from free list
static void remove_from_free_list(block_t* block, int order) {
	if(block_next(block))
		block_next(block)->prev=block->prev;
	if(block->prev)
		block_prev(block)->next=block->next;
	else{
		assert(arena->free_list[order]==block);
		arena->free_list[order]=block_next(block);
	}
}

// Add block to free list
static void add_to_free_list(block_t* block, int order) {
	block->next=block_no(arena->free_list[order]);
	if(arena->free_list[order])
		arena->free_list[order]->prev=block_no(block);
	block->prev=0;
	block->order=order;
	block->is_free=1;
	arena->free_list[order]=block;
}

// Initialize the buddy allocator
void buddy_init(){
    if(arena) return;

	arena=malloc(sizeof(*arena)+POOL_SIZE);
	memset(arena,0,sizeof(*arena));

	arena->memory_pool=(char*)arena+sizeof(*arena);

    
    // Create initial block covering entire pool
    block_t* initial_block = (block_t*)arena->memory_pool;
    add_to_free_list(initial_block, MAX_ORDER);
    
    arena->total_allocator = 0;
    arena->allocator_count = 0;
	allocator=buddy_alloc;
}

// Split a block into two buddies
static void split_block(int order) {
    if (order >= MAX_ORDER || !arena->free_list[order + 1]) {
        return;
    }
    
    // Remove block from higher order list
    block_t* block = arena->free_list[order + 1];
    remove_from_free_list(block, order + 1);
    
    // Split into two buddies
    size_t half_size = get_block_size(order);
    block_t* buddy = (block_t*)((char*)block + half_size);
    
    // Add both halves to current order free list
    add_to_free_list(block, order);
    add_to_free_list(buddy, order);
}

static void* buddy_malloc(size_t size) {
    if (!arena) {
        buddy_init();
    }
    
    if (size == 0) return NULL;
    
    // Add space for block header
    size_t total_size = size + sizeof(block_t);
    int order = get_order(total_size);
    
    if (order > MAX_ORDER) {
		assert(false);
        return NULL; // Too large
    }
    
    // Find available block
    int current_order = order;
    while (current_order <= MAX_ORDER && !arena->free_list[current_order]) {
        current_order++;
    }
    
    if (current_order > MAX_ORDER) {
		fprintf(stderr,"out of memory. total used=%d, +%d\n",arena->total_allocator, get_block_size(order));

		assert(false);
        return NULL; // No memory available
    }
    
    // Split blocks until we get the right size
    while (current_order > order) {
        split_block(current_order - 1);
        current_order--;
    }
    
    // Allocate the block
    block_t* block = arena->free_list[order];
    if (!block) {
		assert(false);
        return NULL;
    }
    
    remove_from_free_list(block, order);
    block->is_free = 0;
    block->order = order;
    
    arena->total_allocator += get_block_size(order);
    arena->allocator_count++;
    
    // Return pointer after header
    return (char*)block + sizeof(block_t);
}

static void buddy_free(void* ptr) {
    if (!ptr || !arena) return;
    
    // Get block header
    block_t* block = (block_t*)((char*)ptr - sizeof(block_t));
    
    if (block->is_free) {
        return; // Double free protection
    }
    
    int order = block->order;
    arena->total_allocator -= get_block_size(order);
    arena->allocator_count--;
    
    // Try to merge with buddy
    while (order < MAX_ORDER) {
        block_t* buddy = (block_t*)get_buddy(block, order);
        
        // Check if buddy is valid and free
        if ((char*)buddy < arena->memory_pool || 
            (char*)buddy >= arena->memory_pool + POOL_SIZE ||
            !buddy->is_free || 
            buddy->order != order) {
            break;
        }
        
        // Remove buddy from free list
        remove_from_free_list(buddy, order);
        
        // Merge blocks (ensure block is the lower address)
        if (buddy < block) {
            block = buddy;
        }
        
        order++;
    }
    
    // Add merged block to appropriate free list
    add_to_free_list(block, order);
}

static void* buddy_realloc(void* ptr, size_t size) {
    if (!ptr) {
        return buddy_malloc(size);
    }
    
    if (size == 0) {
        buddy_free(ptr);
        return NULL;
    }
    
    // Get current block info
    block_t* block = (block_t*)((char*)ptr - sizeof(block_t));
    size_t current_size = get_block_size(block->order) - sizeof(block_t);
    
    // If new size fits in current block, just return same pointer
    size_t needed_size = size + sizeof(block_t);
    if (get_order(needed_size) == block->order) {
        return ptr;
    }
    
    // Allocate new block
    void* new_ptr = buddy_malloc(size);
    if (!new_ptr) {
        return NULL;
    }
    
    // Copy data (copy the smaller of old size or new size)
    size_t copy_size = (current_size < size) ? current_size : size;
    memcpy(new_ptr, ptr, copy_size);
    
    // Free old block
    buddy_free(ptr);
    
    return new_ptr;
}

// Check for memory leaks
void buddy_check_leaks() {
    fprintf(stderr,"\n=== Memory Leak Check ===\n");
    fprintf(stderr,"Total allocated: %zu bytes\n", arena->total_allocator);
    fprintf(stderr,"Active allocations: %zu\n", arena->allocator_count);
    
    if (arena->total_allocator == 0 && arena->allocator_count == 0) {
        fprintf(stderr,"✓ No memory leaks detected!\n");
    } else {
        fprintf(stderr,"✗ Memory leak detected!\n");
        fprintf(stderr,"  Leaked memory: %zu bytes\n", arena->total_allocator);
        fprintf(stderr,"  Leaked blocks: %zu\n", arena->allocator_count);
    }
    
    // Show free list status
    fprintf(stderr,"\nFree list status:\n");
    for (int i = 0; i <= MAX_ORDER; i++) {
        int count = 0;
        block_t* current = arena->free_list[i];
        while (current) {
            count++;
            current = block_next(current);
        }
        if (count > 0) {
            fprintf(stderr,"  Order %2d (%8zu bytes): %d free blocks\n", i, get_block_size(i), count);
        }
    }
}
void buddy_close(){
	allocator=os_alloc;
	if(arena->free_list[MAX_ORDER]){
		free(arena);
		return;
	}
	buddy_check_leaks();
	fprintf(stderr,"memory leaked!");
	return;
}

void* buddy_alloc(void* ptr, size_t len){
	if(len && ptr) return buddy_realloc(ptr,len);
	if(!ptr) return buddy_malloc(len);
	if(ptr) buddy_free(ptr);
	return NULL;
}
void* os_alloc(void* ptr, size_t len){
	if(len && ptr) return realloc(ptr,len);
	if(!ptr) return malloc(len);
	free(ptr);
	return NULL;
}
//void backtrack(struct block* ret) {
//	void *array[10];
//	size_t size;
//	char **strings;
//	size = backtrace(array, 10);
//	strings = backtrace_symbols(array, size);
//	int rest=sizeof(ret->backtrack)-1;
//	int done=0;
//	for(int i=0; i<size; i++){
//		int len=strlen(strings[i]);
//		if(len>rest-1) len=rest-1;
//		memcpy(ret->backtrack+done,strings[i],len);
//		ret->backtrack[len]='\n';
//		rest-=len+1;
//		done+=len+1;
//		if(!rest) break;
//	}
//	ret->backtrack[done]='\0';
//	free(strings);
//}
