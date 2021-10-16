// FILE FOR TEST SYSTEM
#include <stdio.h>
#include "memallocator.h"

void* g_head = NULL;
void* s_pmemory = NULL;
int s_size = 0;

int myabs(int x) {
    return x >= 0 ? x : -x;
}

int* getleftsizeofblock(void* desc) {
    return (int*)desc;
}

void** getblocknext(void* desc) {
    return (void**)((char*)desc + sizeof(int));
}

int* getrightsizeofblock(void* desc) {
    return (int*)((char*)desc + myabs(*getleftsizeofblock(desc)) - sizeof(int));
}

int memgetminimumsize() {
    return sizeof(int) + sizeof(void*) + sizeof(int);
}

int memgetblocksize() {
    return sizeof(int) + sizeof(void*) + sizeof(int); // block have last 4 bytes as its size to realize left-side merging anti-fragmentation feature
}

int meminit(void* pMemory, int size) {
    // 0 byte block should not be available to init similar to memalloc behaviour 
    if (size <= memgetminimumsize())
        return 0;
    void* desc = pMemory;
    *getleftsizeofblock(desc) = size;
    *getblocknext(desc) = NULL;
    *getrightsizeofblock(desc) = size;
    g_head = desc;
    s_pmemory = pMemory;
    s_size = size;
    return size;
}

void memdone() {
    void* iter = s_pmemory;
    while ((char*)iter < (char*)s_pmemory + s_size) {
        if (*getleftsizeofblock(iter) < 0)
            // info for user so ptr not on desc but on user's part of block
            fprintf(stderr, "[MEMORY LEAK] block at 0x%p\n", (char*)iter + sizeof(int) + sizeof(void*)); 
        iter = (void*)((char*)iter + myabs(*getleftsizeofblock(iter)));
    }
}

void* memalloc(int size) {
    if (size < 1 || size > s_size - memgetblocksize())
        return NULL;
    // searching for the best fit block
    int block_found = 0;
    void* iter_prev = NULL;
    void* iter = g_head;
    void* bestfit = iter; // descriptor of best fit block
    void* bestfit_prev = NULL;
    while (iter) {
        if (*getleftsizeofblock(iter) >= memgetblocksize() + size) {
            block_found = 1;
            if (*getleftsizeofblock(iter) < *getleftsizeofblock(bestfit)) {
                bestfit = iter;
                bestfit_prev = iter_prev;
            }
        }
        iter_prev = iter;
        iter = *getblocknext(iter);
    }
    if (!block_found)
        return NULL;
    // if we have memory to set new descriptor
    if (*getleftsizeofblock(bestfit) > memgetblocksize() + size + memgetblocksize()) {
        // filling new descriptor for left part of block
        void* free_block_desc = (void*)((char*)bestfit + memgetblocksize() + size);
        *getleftsizeofblock(free_block_desc) = *getleftsizeofblock(bestfit) - memgetblocksize() - size;
        *getblocknext(free_block_desc) = *getblocknext(bestfit);
        *getrightsizeofblock(free_block_desc) = *getleftsizeofblock(free_block_desc);
        if (bestfit_prev)
            *getblocknext(bestfit_prev) = free_block_desc;
        else
            g_head = free_block_desc;
        // allocating memory for user
        *getleftsizeofblock(bestfit) = memgetblocksize() + size;
    }
    else {
        if (bestfit_prev)
            *getblocknext(bestfit_prev) = *getblocknext(bestfit);
        else
            g_head = *getblocknext(bestfit);
    }
    *getleftsizeofblock(bestfit) = -*getleftsizeofblock(bestfit); // if block is allocated for user its size is negative
    *getrightsizeofblock(bestfit) = *getleftsizeofblock(bestfit);
    *getblocknext(bestfit) = NULL;
    // returning ptr on memory after bestfit descriptor for user usage
    return (void*)((char*)bestfit + sizeof(int) + sizeof(void*));
}

void memfree(void* p) {
    void* pdesc = (void*)((char*)p - sizeof(void*) - sizeof(int));
    void* left_block_desc = NULL;
    if ((char*)pdesc - 1 > (char*)s_pmemory)
        left_block_desc = (void*)((char*)pdesc - myabs(*((int*)pdesc - 1)));
    void* right_block_desc = (void*)(getrightsizeofblock(pdesc) + 1);
    char merged_with_left = 0, merged_with_right = 0;

    *getleftsizeofblock(pdesc) = -*getleftsizeofblock(pdesc);
    *getrightsizeofblock(pdesc) = *getleftsizeofblock(pdesc);

    if (left_block_desc >= s_pmemory)
        if (*getleftsizeofblock(left_block_desc) > 0) {
            merged_with_left = 1;
            *getleftsizeofblock(left_block_desc) += *getleftsizeofblock(pdesc);
            *getrightsizeofblock(left_block_desc) = *getleftsizeofblock(left_block_desc);
            pdesc = left_block_desc;
        }

    if ((char*)right_block_desc < (char*)s_pmemory + s_size)
        if (*getleftsizeofblock(right_block_desc) > 0) {
            merged_with_right = 1;
            if (!merged_with_left) {
                *getblocknext(pdesc) = g_head;
                g_head = pdesc;
            }
            // search to the right_block_desc_prev in free space list
            void* right_block_desc_prev = g_head;
            void* iter = g_head;
            while (iter) {
                if (*getblocknext(iter) == right_block_desc) {
                    right_block_desc_prev = iter;
                    break;
                }
                iter = *getblocknext(iter);
            }
            if (right_block_desc_prev != right_block_desc)
                // skipping right_block_desc in free space list as it will be merged with pdesc
                *getblocknext(right_block_desc_prev) = *getblocknext(right_block_desc);
            else
                g_head = *getblocknext(right_block_desc);
            // updating size
            *getleftsizeofblock(pdesc) += *getleftsizeofblock(right_block_desc);
            *getrightsizeofblock(pdesc) = *getleftsizeofblock(pdesc);
        }

    if (!merged_with_left && !merged_with_right) {
        *getblocknext(pdesc) = g_head;
        g_head = pdesc;
    }
}
