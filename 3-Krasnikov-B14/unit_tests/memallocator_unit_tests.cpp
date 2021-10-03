#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "gtest/gtest.h"
#include "../../lab2/memallocator.h"
#include "../include/memalloc_utils.h"

#define TEST_MEMORY_TEXT_BLOCK_SIZE 10
#define CNULL (void*)0 // NULL is defined as 0 in cpp somewhere so need a c NULL instead

// meminit is being tested with ASSERT_EQ instead of EXPECT_EQ so I can use it in further tests
// this is simple func so it is hard not to use it in tests, if i do so it will be copy-pasted meminit func
TEST(meminit_Test, meminit_meminit_expectValidBlockMarkup) {
    const int TEST_MEMORY_SIZE_INIT = memgetminimumsize() + 1;
    void* ptr = malloc(TEST_MEMORY_SIZE_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_INIT);
    assert(bytes_init);
    desc_t* desc = (desc_t*)ptr;
    ASSERT_EQ(bytes_init, TEST_MEMORY_SIZE_INIT);
    ASSERT_EQ(desc->size, TEST_MEMORY_SIZE_INIT);
    ASSERT_EQ(desc->next, CNULL);
    ASSERT_EQ(*getrightsizeofblock(desc), TEST_MEMORY_SIZE_INIT);
    free(ptr);
}

TEST(meminit_Test, meminit_meminitTooLittleBytes_expectZeroBytesInit) {
    const int TEST_MEMORY_SIZE_NOT_ENOUGH_TO_INIT = memgetminimumsize();
    void* ptr = malloc(TEST_MEMORY_SIZE_NOT_ENOUGH_TO_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_NOT_ENOUGH_TO_INIT);
    EXPECT_EQ(bytes_init, 0);
    free(ptr);
}

TEST(memalloc_Test, memalloc_memallocAllInitMemory_expectSizeIsNegative) {
    const int TEST_MEMORY_SIZE_INIT = memgetminimumsize() + TEST_MEMORY_TEXT_BLOCK_SIZE;
    void* ptr = malloc(TEST_MEMORY_SIZE_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_INIT);
    assert(bytes_init);
    desc_t* desc = (desc_t*)ptr;
    char* a = (char*)memalloc(TEST_MEMORY_TEXT_BLOCK_SIZE);
    EXPECT_EQ(bytes_init, TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(desc->size, -TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(desc->next, CNULL);
    EXPECT_EQ(*getrightsizeofblock(desc), -TEST_MEMORY_SIZE_INIT);
    free(ptr);
}

TEST(memalloc_Test, memalloc_memallocAllInitMemoryWriteSmth_expectBlockInfoNotCorrupted) {
    const int TEST_MEMORY_SIZE_INIT = memgetminimumsize() + TEST_MEMORY_TEXT_BLOCK_SIZE;
    const char sometext[TEST_MEMORY_TEXT_BLOCK_SIZE] = "abccbbfff";
    void* ptr = malloc(TEST_MEMORY_SIZE_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_INIT);
    assert(bytes_init);
    desc_t* desc = (desc_t*)ptr;
    char* a = (char*)memalloc(TEST_MEMORY_TEXT_BLOCK_SIZE);
    // copying smth to *a not more than TEST_MEMORY_TEXT_BLOCK_SIZE so if smth goes wrong block info will be damaged
    strcpy(a, sometext);
    EXPECT_EQ(bytes_init, TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(desc->size, -TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(desc->next, CNULL);
    EXPECT_EQ(*getrightsizeofblock(desc), -TEST_MEMORY_SIZE_INIT);
    EXPECT_TRUE(!strcmp(a, sometext));
    free(ptr);
}

TEST(memalloc_Test, memalloc_memallocAllInitMemoryNextMemallocFail_expectSecondMemallocReturnNull) {
    const int TEST_MEMORY_SIZE_INIT = memgetminimumsize() + TEST_MEMORY_TEXT_BLOCK_SIZE;
    void* ptr = malloc(TEST_MEMORY_SIZE_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_INIT);
    assert(bytes_init);
    desc_t* desc = (desc_t*)ptr;
    char* a = (char*)memalloc(TEST_MEMORY_TEXT_BLOCK_SIZE);
    char* b = (char*)memalloc(TEST_MEMORY_TEXT_BLOCK_SIZE);
    EXPECT_EQ(b, CNULL);
    EXPECT_EQ(bytes_init, TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(desc->size, -TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(desc->next, CNULL);
    EXPECT_EQ(*getrightsizeofblock(desc), -TEST_MEMORY_SIZE_INIT);
    free(ptr);
}

TEST(memalloc_Test, memalloc_memallocInitTextBlockAndCharBlock_expectMemoryAllocedCorrect) {
    const int TEST_MEMORY_SIZE_INIT = memgetminimumsize() + TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize() + sizeof(char);
    void* ptr = malloc(TEST_MEMORY_SIZE_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_INIT);
    assert(bytes_init);
    char* a_text = (char*)memalloc(TEST_MEMORY_TEXT_BLOCK_SIZE);
    char* b_char = (char*)memalloc(sizeof(char));
    desc_t* desc_a = (desc_t*)a_text - 1;
    desc_t* desc_b = (desc_t*)b_char - 1;
    EXPECT_EQ(bytes_init, TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(desc_a->size, -(TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize()));
    EXPECT_EQ(desc_a->next, CNULL);
    EXPECT_EQ(*getrightsizeofblock(desc_a), -(TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize()));
    EXPECT_EQ(desc_a->size, -(TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize()));
    EXPECT_EQ(desc_a->next, CNULL);
    EXPECT_EQ(*getrightsizeofblock(desc_a), -(TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize()));
    free(ptr);
}

TEST(memalloc_Test, memalloc_memallocInitTextBlockAndCharBlock_expectNoBlocksCorrupted) {
    const int TEST_MEMORY_SIZE_INIT = memgetminimumsize() + TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize() + sizeof(char);
    const char sometext[TEST_MEMORY_TEXT_BLOCK_SIZE] = "abccbbfff";
    const char b = 'b';
    void* ptr = malloc(TEST_MEMORY_SIZE_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_INIT);
    assert(bytes_init);
    char* a_text = (char*)memalloc(TEST_MEMORY_TEXT_BLOCK_SIZE);
    char* b_char = (char*)memalloc(sizeof(char));
    desc_t* desc_a = (desc_t*)a_text - 1;
    desc_t* desc_b = (desc_t*)b_char - 1;
    strcpy(a_text, sometext);
    *b_char = b;
    EXPECT_EQ(bytes_init, TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(desc_a->size, -(TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize()));
    EXPECT_EQ(desc_a->next, CNULL);
    EXPECT_EQ(*getrightsizeofblock(desc_a), -(TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize()));
    EXPECT_EQ(desc_a->size, -(TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize()));
    EXPECT_EQ(desc_a->next, CNULL);
    EXPECT_EQ(*getrightsizeofblock(desc_a), -(TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize()));
    EXPECT_TRUE(!strcmp(a_text, sometext));
    EXPECT_EQ(*b_char, b);
    free(ptr);
}
