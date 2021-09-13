#include "gtest/gtest.h"
#include "pqueue.h"
#include "stdlib.h"

#define MAX_WORD_LENGTH 20

TEST(pqueue_push_Test, pqueue_push_pushInEmptyQueue_returnValidVal) {
    pqueue_t* q = pqueue_init();
    char first[MAX_WORD_LENGTH] = "first";
    pqueue_push(q, first, 1);
    EXPECT_TRUE(!strcmp((char*)(q->head->next->data), first));
}

TEST(pqueue_push_Test, pqueue_push_pushManyItems_returnValidItemsInValidOrder) {
    pqueue_t* q = pqueue_init();
    char first[MAX_WORD_LENGTH] = "first";
    char second[MAX_WORD_LENGTH] = "second";
    char third[MAX_WORD_LENGTH] = "third";
    pqueue_push(q, third, 3);
    pqueue_push(q, first, 1);
    pqueue_push(q, second, 2);
    EXPECT_TRUE(!strcmp((char*)q->head->next->data, first));
    EXPECT_TRUE(!strcmp((char*)q->head->next->next->data, second));
    EXPECT_TRUE(!strcmp((char*)q->head->next->next->next->data, third));
}

TEST(pqueue_isempty_Test, pqueue_isempty_isemptyEmptyQueue_returnTrue) {
    pqueue_t* q = pqueue_init();
    EXPECT_TRUE(pqueue_isempty(q));
}

TEST(pqueue_pop_Test, pqueue_pop_popOneItem_returnValidVal) {
    pqueue_t* q = pqueue_init();
    char first[MAX_WORD_LENGTH] = "first";
    node_t* new_node = (node_t*)malloc(sizeof(node_t));
    new_node->next = NULL;
    new_node->data = first;
    new_node->priority = 1;
    q->head->next = new_node;
    EXPECT_TRUE(!strcmp((char*)(pqueue_pop(q)), first));
    EXPECT_EQ(q->head->next, nullptr);
}

TEST(pqueue_pop_Test, pqueue_pop_popManyItems_returnValidVal) {
    pqueue_t* q = pqueue_init();
    char first[MAX_WORD_LENGTH] = "first";
    char second[MAX_WORD_LENGTH] = "second";
    char third[MAX_WORD_LENGTH] = "third";

    node_t* new_node = (node_t*)malloc(sizeof(node_t));
    new_node->next = NULL;
    new_node->data = first;
    new_node->priority = 1;
    q->head->next = new_node;

    new_node = (node_t*)malloc(sizeof(node_t));
    new_node->next = NULL;
    new_node->data = second;
    new_node->priority = 2;
    q->head->next->next = new_node;

    new_node = (node_t*)malloc(sizeof(node_t));
    new_node->next = NULL;
    new_node->data = third;
    new_node->priority = 3;
    q->head->next->next->next = new_node;

    EXPECT_TRUE(!strcmp((char*)(pqueue_pop(q)), first));
    EXPECT_TRUE(!strcmp((char*)(pqueue_pop(q)), second));
    EXPECT_TRUE(!strcmp((char*)(pqueue_pop(q)), third));
    EXPECT_EQ(q->head->next, nullptr);
}

TEST(pqueue_pop_Test, pqueue_pop_popEmptyQueue_returnNULL) {
    pqueue_t* q = pqueue_init();
    EXPECT_EQ(pqueue_pop(q), nullptr);
}
