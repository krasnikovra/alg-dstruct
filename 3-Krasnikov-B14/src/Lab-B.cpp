// THIS FILE IS COPY OF 'memalloc.c' ADJUSTED FOR LAB TEST SYSTEM

// COPY-PASTED FROM memallocator.h FILE
// --------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

// Init memory system with memory block pMemory.
int meminit(void *pMemory, int size);

// You can implement memory leak checks here
void memdone();

// Allocate memory block of size 'size'.
// Returns pointer to memory block is success, 0 otherwise
void *memalloc(int size);

// Free memory previously allocated by memalloc
void memfree(void *p);

// Return minimum size in bytes of the memory pool to allocate 0-bytes block
// use case:
// void *p   = 0;
// void *ptr = malloc(memgetminimumsize() + 1);
// meminit(ptr, memgetminimumsize() + 1)
// p = memalloc(1); // Success!
// memfree(p);
// memdone();
// free(ptr);
int memgetminimumsize();


// Returns size in bytes of additional information per allocation
// use case:
// void *p1 = 0, *p2 = 0;
// int  memsize = memgetminimumsize() + memgetblocksize() + 2;
// void *ptr = malloc(memsize);
// meminit(ptr, memsize())
// p1 = memalloc(1); // Success!
// p2 = memalloc(1); // Success!
// memfree(p2);
// memfree(p1);
// memdone();
// free(ptr);
int memgetblocksize();
#ifdef __cplusplus
}
#endif
// -------------------------------------------

// COPY-PASTED FROM memallocator_utils.h FILE
// --------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

int myabs(int x);
int* getrightsizeofblock(void* desc);
int* getleftsizeofblock(void* desc);
void** getblocknext(void* desc);

extern void* s_head;

#ifdef __cplusplus
}
#endif
// ---------------------------------------

#include <stdio.h>

void* s_head = NULL;
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

int meminit(void* pMemory, int size) {
    // 0 byte block should not be available to init similar to memalloc behaviour 
    if (size <= memgetminimumsize())
        return 0;
    void* desc = pMemory;
    *getleftsizeofblock(desc) = size;
    *getblocknext(desc) = NULL;
    *getrightsizeofblock(desc) = size;
    s_head = desc;
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
    if (size < 1)
        return NULL;
    // searching for the best fit block
    int block_found = 0;
    void* iter_prev = NULL;
    void* iter = s_head;
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
            s_head = free_block_desc;
        // allocating memory for user
        *getleftsizeofblock(bestfit) = memgetblocksize() + size;
    }
    else {
        if (bestfit_prev)
            *getblocknext(bestfit_prev) = *getblocknext(bestfit);
        else
            s_head = *getblocknext(bestfit);
    }
    *getleftsizeofblock(bestfit) = -*getleftsizeofblock(bestfit); // if block is allocated for user its size is negative
    *getrightsizeofblock(bestfit) = *getleftsizeofblock(bestfit);
    *getblocknext(bestfit) = NULL;
    // returning ptr on memory after bestfit descriptor for user usage
    return (void*)((char*)bestfit + sizeof(int) + sizeof(void*));
}

void memfree(void* p) {
    void* pdesc = (void*)((char*)p - sizeof(void*) - sizeof(int));
    *getleftsizeofblock(pdesc) = -*getleftsizeofblock(pdesc); // converting size to positive which means block is not allocated for user now
    *getrightsizeofblock(pdesc) = *getleftsizeofblock(pdesc);
    *getblocknext(pdesc) = s_head;
    s_head = pdesc;
    // anti-fragmentation feauture: if we have free block in the right of freed we need to merge them in one block
    void* right_block_desc = (void*)((char*)pdesc + *getleftsizeofblock(pdesc));
    // if we have any block to the right side
    if ((char*)right_block_desc < (char*)s_pmemory + s_size)
        // and this block is free
        if (*getleftsizeofblock(right_block_desc) > 0) {
            // search to the right_block_desc_prev in free space list
            void* right_block_desc_prev = s_head;
            void* iter = s_head;
            while (iter) {
                if (*getblocknext(iter) == right_block_desc) {
                    right_block_desc_prev = iter;
                    break;
                }
                iter = *getblocknext(iter);
            }
            // skipping right_block_desc in free space list as it will be merged with pdesc
            *getblocknext(right_block_desc_prev) = *getblocknext(right_block_desc);
            *getleftsizeofblock(pdesc) += *getleftsizeofblock(right_block_desc);
            *getrightsizeofblock(pdesc) = *getleftsizeofblock(pdesc);
        }
    // anti-fragmentation feauture: if we have free block in the left of freed we need to merge them in one block
    // go backwards for one byte to check if we are on the edge of memory or have any block to the left
    if ((char*)pdesc - 1 > (char*)s_pmemory) {
        int left_block_size = *(int*)((char*)pdesc - sizeof(int));
        void* left_block_desc = (void*)((char*)pdesc - myabs(left_block_size));
        // if this block is free
        if (*getleftsizeofblock(left_block_desc) > 0) {
            // search to the left_block_desc_prev in free space list
            void* left_block_desc_prev = s_head;
            void* iter = s_head;
            while (iter) {
                if (*getblocknext(iter) == left_block_desc) {
                    left_block_desc_prev = iter;
                    break;
                }
                iter = *getblocknext(iter);
            }
            // skipping left_block_desc in free space list as it will be merged with pdesc
            *getblocknext(left_block_desc_prev) = *getblocknext(left_block_desc);
            // left_block_desc is new head as pdesc was merged into it
            s_head = left_block_desc;
            *getblocknext(s_head) = *getblocknext(pdesc);
            // updating size
            *getleftsizeofblock(left_block_desc) += *getleftsizeofblock(pdesc);
            *getrightsizeofblock(left_block_desc) = *getleftsizeofblock(left_block_desc);
        }
    }
}

int memgetminimumsize() {
    return sizeof(int) + sizeof(void*) + sizeof(int);
}

int memgetblocksize() {
    return sizeof(int) + sizeof(void*) + sizeof(int); // block have last 4 bytes as its size to realize left-side merging anti-fragmentation feature
}
