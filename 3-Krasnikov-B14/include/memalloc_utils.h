#ifndef __MEMALLOC_UTILS_H__
#define __MEMALLOC_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

int myabs(int x);
int* getrightsizeofblock(void* desc);
int* getleftsizeofblock(void* desc);
void** getblocknext(void* desc);

extern void* g_head;

#ifdef __cplusplus
}
#endif

#endif
