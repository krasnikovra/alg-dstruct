#include <stdio.h>
#include "../../lab2/memallocator.h"

typedef struct {
    int size; // size of block includes size of descriptor too
    void* next;
} desc_t; // memory block descriptor

static desc_t* s_head = NULL;
static void* s_pmemory = NULL;
static int s_size = 0;

static int _myabs(int x) {
    return x >= 0 ? x : -x;
}

int meminit(void* pMemory, int size) {
    if (size < memgetminimumsize())
        return 0;
    desc_t* desc = (desc_t*)pMemory;
    desc->size = size;
    desc->next = NULL;
    s_head = desc;
    s_pmemory = pMemory;
    s_size = size;
    return size;
}

void memdone() {
    desc_t* iter = (desc_t*)s_pmemory;
    while ((char*)iter < (char*)s_pmemory + s_size) {
        if (iter->size < 0)
            // info for user so ptr not on desc but on user's part of block
            printf("[MEMORY LEAK] block at 0x%p\n", iter + 1); 
        iter = (desc_t*)((char*)iter + _myabs(iter->size));
    }
}

void* memalloc(int size) {
    if (size < 1)
        return NULL;
    // searching for the best fit block
    int block_found = 0;
    desc_t* iter_prev = NULL;
    desc_t* iter = s_head;
    desc_t* bestfit = iter; // descriptor of best fit block
    desc_t* bestfit_prev = NULL;
    while (iter) {
        if (iter->size >= memgetblocksize() + size) {
            block_found = 1;
            if (iter->size < bestfit->size) {
                bestfit = iter;
                bestfit_prev = iter_prev;
            }
        }
        iter_prev = iter;
        iter = iter->next;
    }
    if (!block_found)
        return NULL;
    // if we have memory to set new descriptor
    if (bestfit->size >= memgetblocksize() + size + memgetblocksize()) {
        // filling new descriptor for left part of block
        desc_t* free_block_desc = (desc_t*)((char*)bestfit + memgetblocksize() + size);
        free_block_desc->size = bestfit->size - memgetblocksize() - size;
        free_block_desc->next = bestfit->next;
        if (bestfit_prev)
            bestfit_prev->next = free_block_desc;
        else
            s_head = free_block_desc;
        // allocating memory for user
        bestfit->size = memgetblocksize() + size;
    }
    else {
        if (bestfit_prev)
            bestfit_prev->next = bestfit->next;
        else
            s_head = bestfit->next;
    }
    bestfit->size = -bestfit->size; // if block is allocated for user its size is negative
    bestfit->next = NULL;
    // returning ptr on memory after bestfit descriptor for user usage
    return (void*)(bestfit + 1);
}

void memfree(void* p) {
    desc_t* pdesc = (desc_t*)p - 1;
    pdesc->size = -pdesc->size; // converting size to positive which means block is not allocated for user now
    pdesc->next = s_head;
    s_head = pdesc;
    // anti-fragmentation feauture: if we have free block in the right of freed we need to merge them in one block
    desc_t* right_block_desc = (desc_t*)((char*)pdesc + pdesc->size);
    // if we have any block to the right side
    if ((char*)right_block_desc < (char*)s_pmemory + s_size)
        //and this block is free
        if (right_block_desc->size > 0) {
            // search to the right_block_desc_prev in free space list
            desc_t* right_block_desc_prev = s_head;
            desc_t* iter = s_head;
            while (iter)
                if (iter->next == right_block_desc) {
                    right_block_desc_prev = iter;
                    break;
                }
            // skipping right_block_desc in free space list as it will be merged with pdesc
            right_block_desc_prev->next = right_block_desc->next;
            pdesc->size += right_block_desc->size;
        }
}

int memgetminimumsize() {
    return sizeof(desc_t);
}

int memgetblocksize() {
    return sizeof(desc_t);
}