#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <execinfo.h>
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

struct block* block_next(struct block* in);
struct block* block_prev(struct block* in);
int block_no(struct block* in);
void buddy_init();
void buddy_check_leaks() ;
void buddy_close();
void* buddy_alloc(void* ptr, size_t len);
void* os_alloc(void* ptr, size_t len);
