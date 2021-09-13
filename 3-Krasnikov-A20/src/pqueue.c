#include <stdlib.h>
#include "pqueue.h"

#ifdef __cplusplus
extern "C" {
#endif

pqueue_t* pqueue_init(void) {
    node_t* helper = (node_t*)malloc(sizeof(node_t));
    if (!helper)
        return NULL;
    helper->next = NULL;
    helper->data = NULL;
    helper->priority = 0;
    pqueue_t* pqueue = (pqueue_t*)malloc(sizeof(pqueue_t));
    if (!pqueue)
        return NULL; // returns NULL if mem alloc failed which is similar to malloc return
    pqueue->head = helper;
    return pqueue;
}

error_t pqueue_push(pqueue_t* pqueue, void* data, unsigned priority) {
    node_t* iter = pqueue->head->next;
    node_t* iter_prev = pqueue->head;
    while(iter != NULL && iter->priority < priority){
        iter_prev = iter;
        iter = iter->next;
    }
    // Add after iter_prev
    node_t* new_node = (node_t*)malloc(sizeof(node_t));
    if (!new_node)
        return (error_t)ERR_MEM_ALLOC_FAILED;
    new_node->data = data;
    new_node->priority = priority;
    new_node->next = iter;
    iter_prev->next = new_node;
    return (error_t)ERR_OK;
}

char pqueue_isempty(pqueue_t* pqueue) {
    return !pqueue->head->next;
}

void* pqueue_pop(pqueue_t* pqueue) {
    if (pqueue_isempty(pqueue))
        return NULL;
    void* data = pqueue->head->next->data;
    node_t* temp = pqueue->head->next;
    pqueue->head->next = temp->next;
    free(temp);
    return data;
}

void pqueue_destroy(pqueue_t* pqueue) {
    node_t* iter = pqueue->head;
    while (iter) {
        node_t* temp = iter;
        iter = iter->next;
        free(temp);
    }
    free(pqueue);
}

#ifdef __cplusplus
}
#endif