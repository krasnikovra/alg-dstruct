#ifndef __MEMALLOC_UTILS_H__
#define __MEMALLOC_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int size; // size of block includes size of descriptor too
    void* next;
} desc_t; // memory block descriptor

int myabs(int x);
int* getrightsizeofblock(desc_t* desc);

#ifdef __cplusplus
}
#endif

#endif
