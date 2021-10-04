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

TEST(memalloc_Test, memalloc_memallocAllocatingBestFitBlock_expectOneByteBlockForCharAllocated) {
    const int TEST_MEMORY_SIZE_INIT = memgetminimumsize() + TEST_MEMORY_TEXT_BLOCK_SIZE + 2 * memgetblocksize() + 2 * sizeof(char);
    const char sometext[TEST_MEMORY_TEXT_BLOCK_SIZE] = "abccbbfff";
    const char b = 'b';
    void* ptr = malloc(TEST_MEMORY_SIZE_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_INIT);
    assert(bytes_init);
    // making a memory markup which is: free TEST_MEMORY_TEXT_BLOCK_SIZE block - allocated char block - free char block
    // expect that memalloc returns pointer to a free char block for memalloc(sizeof(char))
    // first free text block
    desc_t* free_text_block_desc = (desc_t*)ptr;
    s_head = free_text_block_desc;
    free_text_block_desc->size = TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize();
    free_text_block_desc->next = CNULL; // reassign this to the third block when its will be defined
    *getrightsizeofblock(free_text_block_desc) = free_text_block_desc->size;
    // second allocated char block
    desc_t* allocated_char_block_desc = (desc_t*)((char*)free_text_block_desc + myabs(free_text_block_desc->size));
    allocated_char_block_desc->size = -((int)sizeof(char) + memgetblocksize()); // block is allocated so size is negative
    allocated_char_block_desc->next = CNULL;
    *getrightsizeofblock(allocated_char_block_desc) = allocated_char_block_desc->size;
    // third free char block
    desc_t* free_char_block_desc = (desc_t*)((char*)allocated_char_block_desc + myabs(allocated_char_block_desc->size));
    free_char_block_desc->size = (int)sizeof(char) + memgetblocksize();
    free_char_block_desc->next = CNULL;
    *getrightsizeofblock(free_char_block_desc) = free_char_block_desc->size;
    // reassigning free_text_block_desc next to free_char_block_desc to make simple free blocks list
    free_text_block_desc->next = (void*)free_char_block_desc;
    // memalloc a free char block
    void* memallocated_char_block_for_user_ptr = memalloc((int)sizeof(char));
    EXPECT_EQ(memallocated_char_block_for_user_ptr, (void*)(free_char_block_desc + 1));
}

TEST(memfree_Test, memfree_memfreeFreeOneBlock_expectSizeIsPositive) {
    const int TEST_MEMORY_SIZE_INIT = memgetminimumsize() + sizeof(char);
    void* ptr = malloc(TEST_MEMORY_SIZE_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_INIT);
    assert(bytes_init);
    // making all init memory allocated block
    desc_t* all_init_memory_block_desc = (desc_t*)ptr;
    all_init_memory_block_desc->size = -all_init_memory_block_desc->size;
    all_init_memory_block_desc->next = CNULL;
    *getrightsizeofblock(all_init_memory_block_desc) = all_init_memory_block_desc->size;
    // freeing this block
    void* user_ptr_to_block = (void*)(all_init_memory_block_desc + 1);
    memfree(user_ptr_to_block);
    EXPECT_EQ(all_init_memory_block_desc->size, myabs(all_init_memory_block_desc->size));
    EXPECT_EQ(*getrightsizeofblock(all_init_memory_block_desc), myabs(all_init_memory_block_desc->size));
}

TEST(memfree_Test, memfree_memfreeFreeBlockWhereRightBlockIsFree_expectBlocksMerged) {
    const int TEST_MEMORY_SIZE_INIT = memgetminimumsize() + memgetblocksize() + 2 * sizeof(char);
    void* ptr = malloc(TEST_MEMORY_SIZE_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_INIT);
    assert(bytes_init);
    // first block is allocated
    desc_t* allocated_char_block_desc = (desc_t*)(ptr);
    allocated_char_block_desc->size = -((int)sizeof(char) + memgetblocksize()); // block is allocated so size is negative
    allocated_char_block_desc->next = CNULL;
    *getrightsizeofblock(allocated_char_block_desc) = allocated_char_block_desc->size;
    // second block is free
    desc_t* free_char_block_desc = (desc_t*)((char*)allocated_char_block_desc + myabs(allocated_char_block_desc->size));
    s_head = free_char_block_desc;
    free_char_block_desc->size = (int)sizeof(char) + memgetblocksize();
    free_char_block_desc->next = CNULL;
    *getrightsizeofblock(free_char_block_desc) = free_char_block_desc->size;
    // now freeing and expect memory state similar with state after meminit
    void* user_ptr_to_allocated_block = (void*)(allocated_char_block_desc + 1);
    memfree(user_ptr_to_allocated_block);
    EXPECT_EQ(allocated_char_block_desc->size, bytes_init);
    EXPECT_EQ(*getrightsizeofblock(allocated_char_block_desc), bytes_init);
    EXPECT_EQ(s_head, (desc_t*)ptr);
}

TEST(memfree_Test, memfree_memfreeFreeBlockWhereLeftBlockIsFree_expectBlocksMerged) {
    const int TEST_MEMORY_SIZE_INIT = memgetminimumsize() + memgetblocksize() + 2 * sizeof(char);
    void* ptr = malloc(TEST_MEMORY_SIZE_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_INIT);
    assert(bytes_init);
    // first block is free
    desc_t* free_char_block_desc = (desc_t*)(ptr);
    s_head = free_char_block_desc;
    free_char_block_desc->size = (int)sizeof(char) + memgetblocksize();
    free_char_block_desc->next = CNULL;
    *getrightsizeofblock(free_char_block_desc) = free_char_block_desc->size;
    // second block is allocated
    desc_t* allocated_char_block_desc = (desc_t*)((char*)free_char_block_desc + myabs(free_char_block_desc->size));
    allocated_char_block_desc->size = -((int)sizeof(char) + memgetblocksize()); // block allocated so its size is negative
    allocated_char_block_desc->next = CNULL;
    *getrightsizeofblock(allocated_char_block_desc) = allocated_char_block_desc->size;
    // now freeing and expect memory state similar with state after meminit
    void* user_ptr_to_allocated_block = (void*)(allocated_char_block_desc + 1);
    memfree(user_ptr_to_allocated_block);
    EXPECT_EQ(free_char_block_desc->size, bytes_init);
    EXPECT_EQ(*getrightsizeofblock(free_char_block_desc), bytes_init);
    EXPECT_EQ(s_head, (desc_t*)ptr);
}

TEST(memfree_Test, memfree_memfreeFreeBlockBetweenTwoFreeBlocks_expectBlocksMerged) {
    const int TEST_MEMORY_SIZE_INIT = memgetminimumsize() + 2 * memgetblocksize() + 3 * sizeof(char);
    void* ptr = malloc(TEST_MEMORY_SIZE_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_INIT);
    assert(bytes_init);
    // first block is free
    desc_t* first_free_char_block_desc = (desc_t*)(ptr);
    s_head = first_free_char_block_desc;
    first_free_char_block_desc->size = (int)sizeof(char) + memgetblocksize();
    first_free_char_block_desc->next = CNULL;
    *getrightsizeofblock(first_free_char_block_desc) = first_free_char_block_desc->size;
    // second block is allocated
    desc_t* allocated_char_block_desc = (desc_t*)((char*)first_free_char_block_desc + myabs(first_free_char_block_desc->size));
    allocated_char_block_desc->size = -((int)sizeof(char) + memgetblocksize()); // block allocated so its size is negative
    allocated_char_block_desc->next = CNULL;
    *getrightsizeofblock(allocated_char_block_desc) = allocated_char_block_desc->size;
    // third block is free
    desc_t* third_free_char_block_desc = (desc_t*)((char*)allocated_char_block_desc + myabs(allocated_char_block_desc->size));
    third_free_char_block_desc->size = (int)sizeof(char) + memgetblocksize();
    third_free_char_block_desc->next = CNULL;
    *getrightsizeofblock(third_free_char_block_desc) = third_free_char_block_desc->size;
    first_free_char_block_desc->next = third_free_char_block_desc;
    // now freeing and expect memory state similar with state after meminit
    void* user_ptr_to_allocated_block = (void*)(allocated_char_block_desc + 1);
    memfree(user_ptr_to_allocated_block);
    EXPECT_EQ(first_free_char_block_desc->size, bytes_init);
    EXPECT_EQ(*getrightsizeofblock(first_free_char_block_desc), bytes_init);
    EXPECT_EQ(s_head, (desc_t*)ptr);
}