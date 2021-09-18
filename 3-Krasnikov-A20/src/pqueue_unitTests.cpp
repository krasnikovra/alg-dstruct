#include "gtest/gtest.h"
#include "pqueue.h"
#include <stdlib.h>
#include <assert.h>

#define MAX_WORD_LENGTH 20
#define PQUEUE_INIT_ON_STACK(pqueue_name) \
        node_t helper; \
        helper.next = NULL; \
        helper.data = NULL; \
        helper.priority = 0; \
        pqueue_t pqueue_on_stack; \
        pqueue_on_stack.head = &helper; \
        pqueue_t* (pqueue_name) = &pqueue_on_stack

TEST(pqueue_init_Test, pqueue_init_init_returnValidVals) {
    pqueue_t* q = pqueue_init();
    EXPECT_TRUE(q);
    EXPECT_TRUE(q->head);
    EXPECT_TRUE(!q->head->next);
    EXPECT_TRUE(!q->head->data);
    EXPECT_TRUE(!q->head->priority);
    pqueue_destroy(q);
}

TEST(pqueue_push_Test, pqueue_push_pushInEmptyQueue_returnValidVal) {
    PQUEUE_INIT_ON_STACK(q);
    char first[MAX_WORD_LENGTH] = "first";
    pqueue_push(q, first, 1);
    EXPECT_TRUE(!strcmp((char*)(q->head->next->data), first));
}

TEST(pqueue_push_Test, pqueue_push_pushManyItems_returnValidItemsInValidOrder) {
    PQUEUE_INIT_ON_STACK(q);
    char first[MAX_WORD_LENGTH] = "first";
    char second[MAX_WORD_LENGTH] = "second";
    char third[MAX_WORD_LENGTH] = "third";
    pqueue_push(q, third, 3);
    pqueue_push(q, first, 1);
    EXPECT_TRUE(!strcmp((char*)q->head->next->data, first));
    EXPECT_TRUE(!strcmp((char*)q->head->next->next->data, third));
    pqueue_push(q, second, 2);
    EXPECT_TRUE(!strcmp((char*)q->head->next->data, first));
    EXPECT_TRUE(!strcmp((char*)q->head->next->next->data, second));
    EXPECT_TRUE(!strcmp((char*)q->head->next->next->next->data, third));
    free(q->head->next->next->next);
    free(q->head->next->next);
    free(q->head->next);
}

TEST(pqueue_push_Test, pqueue_push_pushManyItemsSamePriorities_returnValidItemsInValidOrder) {
    PQUEUE_INIT_ON_STACK(q);
    char first[MAX_WORD_LENGTH] = "first";
    char second[MAX_WORD_LENGTH] = "second";
    char third[MAX_WORD_LENGTH] = "third";
    pqueue_push(q, first, 0);
    pqueue_push(q, second, 0);
    pqueue_push(q, third, 0);
    EXPECT_TRUE(!strcmp((char*)q->head->next->data, first));
    EXPECT_TRUE(!strcmp((char*)q->head->next->next->data, second));
    EXPECT_TRUE(!strcmp((char*)q->head->next->next->next->data, third));
    free(q->head->next->next->next);
    free(q->head->next->next);
    free(q->head->next);
}

TEST(pqueue_isempty_Test, pqueue_isempty_isemptyNotEmptyQueue_returnFalse) {
    PQUEUE_INIT_ON_STACK(q);
    char first[MAX_WORD_LENGTH] = "first";
    node_t new_node;
    new_node.next = NULL;
    new_node.data = first;
    new_node.priority = 1;
    q->head->next = &new_node;
    EXPECT_TRUE(!pqueue_isempty(q));
}

TEST(pqueue_isempty_Test, pqueue_isempty_isemptyEmptyQueue_returnTrue) {
    PQUEUE_INIT_ON_STACK(q);
    EXPECT_TRUE(pqueue_isempty(q));
}

TEST(pqueue_pop_Test, pqueue_pop_popOneItem_returnValidVal) {
    PQUEUE_INIT_ON_STACK(q);
    char first[MAX_WORD_LENGTH] = "first";
    node_t* new_node = (node_t*)malloc(sizeof(node_t));
    assert(new_node);
    new_node->next = NULL;
    new_node->data = first;
    new_node->priority = 1;
    q->head->next = new_node;
    EXPECT_TRUE(!strcmp((char*)(pqueue_pop(q)), first));
    EXPECT_TRUE(!q->head->next);
}

TEST(pqueue_pop_Test, pqueue_pop_popManyItems_returnValidVal) {
    PQUEUE_INIT_ON_STACK(q);
    char first[MAX_WORD_LENGTH] = "first";
    char second[MAX_WORD_LENGTH] = "second";
    char third[MAX_WORD_LENGTH] = "third";

    node_t* new_node = (node_t*)malloc(sizeof(node_t));
    assert(new_node);
    new_node->next = NULL;
    new_node->data = first;
    new_node->priority = 1;
    q->head->next = new_node;

    new_node = (node_t*)malloc(sizeof(node_t));
    assert(new_node);
    new_node->next = NULL;
    new_node->data = second;
    new_node->priority = 2;
    q->head->next->next = new_node;

    new_node = (node_t*)malloc(sizeof(node_t));
    assert(new_node);
    new_node->next = NULL;
    new_node->data = third;
    new_node->priority = 3;
    q->head->next->next->next = new_node;

    EXPECT_TRUE(!strcmp((char*)(pqueue_pop(q)), first));
    EXPECT_TRUE(!strcmp((char*)(pqueue_pop(q)), second));
    EXPECT_TRUE(!strcmp((char*)(pqueue_pop(q)), third));
    EXPECT_TRUE(!q->head->next);
}

TEST(pqueue_pop_Test, pqueue_pop_popEmptyQueue_returnNULL) {
    PQUEUE_INIT_ON_STACK(q);
    EXPECT_TRUE(!pqueue_pop(q));
}
