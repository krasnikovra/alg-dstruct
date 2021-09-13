#ifndef _PQUEUE_H
#define _PQUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct node_t {
    void* data;
    unsigned priority;
    struct node_t* next;
} node_t;

typedef struct {
    node_t* head;
} pqueue_t; // pqueue - priority queue

typedef enum {
    ERR_OK,
    ERR_MEM_ALLOC_FAILED,
} error_t;

pqueue_t* pqueue_init(void);
error_t pqueue_push(pqueue_t* pqueue, void* data, unsigned priority);
char pqueue_isempty(pqueue_t* pqueue);
void* pqueue_pop(pqueue_t* pqueue);
void pqueue_destroy(pqueue_t* pqueue);

#ifdef __cplusplus
}
#endif

#endif