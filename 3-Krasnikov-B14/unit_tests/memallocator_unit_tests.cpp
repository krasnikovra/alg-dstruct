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
    void* desc = ptr;
    ASSERT_EQ(bytes_init, TEST_MEMORY_SIZE_INIT);
    ASSERT_EQ(*getleftsizeofblock(desc) , TEST_MEMORY_SIZE_INIT);
    ASSERT_EQ(*getblocknext(desc), CNULL);
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
    void* desc = ptr;
    char* a = (char*)memalloc(TEST_MEMORY_TEXT_BLOCK_SIZE);
    EXPECT_EQ(bytes_init, TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(*getleftsizeofblock(desc), -TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(*getblocknext(desc), CNULL);
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
    void* desc = ptr;
    char* a = (char*)memalloc(TEST_MEMORY_TEXT_BLOCK_SIZE);
    // copying smth to *a not more than TEST_MEMORY_TEXT_BLOCK_SIZE so if smth goes wrong block info will be damaged
    strcpy(a, sometext);
    EXPECT_EQ(bytes_init, TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(*getleftsizeofblock(desc), -TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(*getblocknext(desc), CNULL);
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
    void* desc = ptr;
    char* a = (char*)memalloc(TEST_MEMORY_TEXT_BLOCK_SIZE);
    char* b = (char*)memalloc(TEST_MEMORY_TEXT_BLOCK_SIZE);
    EXPECT_EQ(b, CNULL);
    EXPECT_EQ(bytes_init, TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(*getleftsizeofblock(desc), -TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(*getblocknext(desc), CNULL);
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
    void* desc_a = (void*)((char*)a_text - sizeof(int) - sizeof(void*));
    void* desc_b = (void*)((char*)b_char - sizeof(int) - sizeof(void*));
    EXPECT_EQ(bytes_init, TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(*getleftsizeofblock(desc_a), -(TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize()));
    EXPECT_EQ(*getblocknext(desc_a), CNULL);
    EXPECT_EQ(*getrightsizeofblock(desc_a), -(TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize()));
    EXPECT_EQ(*getleftsizeofblock(desc_b), -((int)sizeof(char) + memgetblocksize()));
    EXPECT_EQ(*getblocknext(desc_b), CNULL);
    EXPECT_EQ(*getrightsizeofblock(desc_b), -((int)sizeof(char) + memgetblocksize()));
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
    void* desc_a = (void*)((char*)a_text - sizeof(int) - sizeof(void*));
    void* desc_b = (void*)((char*)b_char - sizeof(int) - sizeof(void*));
    strcpy(a_text, sometext);
    *b_char = b;
    EXPECT_EQ(bytes_init, TEST_MEMORY_SIZE_INIT);
    EXPECT_EQ(*getleftsizeofblock(desc_a), -(TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize()));
    EXPECT_EQ(*getblocknext(desc_a), CNULL);
    EXPECT_EQ(*getrightsizeofblock(desc_a), -(TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize()));
    EXPECT_EQ(*getleftsizeofblock(desc_b), -((int)sizeof(char) + memgetblocksize()));
    EXPECT_EQ(*getblocknext(desc_b), CNULL);
    EXPECT_EQ(*getrightsizeofblock(desc_b), -((int)sizeof(char) + memgetblocksize()));
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
    void* free_text_block_desc = ptr;
    s_head = free_text_block_desc;
    *getleftsizeofblock(free_text_block_desc) = TEST_MEMORY_TEXT_BLOCK_SIZE + memgetblocksize();
    *getblocknext(free_text_block_desc) = CNULL; // reassign this to the third block when its will be defined
    *getrightsizeofblock(free_text_block_desc) = *getleftsizeofblock(free_text_block_desc);
    // second allocated char block
    void* allocated_char_block_desc = (void*)((char*)free_text_block_desc + myabs(*getleftsizeofblock(free_text_block_desc)));
    *getleftsizeofblock(allocated_char_block_desc) = -((int)sizeof(char) + memgetblocksize()); // block is allocated so size is negative
    *getblocknext(allocated_char_block_desc) = CNULL;
    *getrightsizeofblock(allocated_char_block_desc) = *getleftsizeofblock(allocated_char_block_desc);
    // third free char block
    void* free_char_block_desc = (void*)((char*)allocated_char_block_desc + myabs(*getleftsizeofblock(allocated_char_block_desc)));
    *getleftsizeofblock(free_char_block_desc) = (int)sizeof(char) + memgetblocksize();
    *getblocknext(free_char_block_desc) = CNULL;
    *getrightsizeofblock(free_char_block_desc) = *getleftsizeofblock(free_char_block_desc);
    // reassigning free_text_block_desc next to free_char_block_desc to make simple free blocks list
    *getblocknext(free_text_block_desc) = free_char_block_desc;
    // memalloc a free char block
    void* memallocated_char_block_for_user_ptr = memalloc((int)sizeof(char));
    EXPECT_EQ((char*)memallocated_char_block_for_user_ptr, (char*)free_char_block_desc + sizeof(int) + sizeof(void*));
    free(ptr);
}

TEST(memfree_Test, memfree_memfreeFreeOneBlock_expectSizeIsPositive) {
    const int TEST_MEMORY_SIZE_INIT = memgetminimumsize() + sizeof(char);
    void* ptr = malloc(TEST_MEMORY_SIZE_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_INIT);
    assert(bytes_init);
    // making all init memory allocated block
    void* all_init_memory_block_desc = ptr;
    *getleftsizeofblock(all_init_memory_block_desc) = -*getleftsizeofblock(all_init_memory_block_desc);
    *getblocknext(all_init_memory_block_desc) = CNULL;
    *getrightsizeofblock(all_init_memory_block_desc) = *getleftsizeofblock(all_init_memory_block_desc);
    // freeing this block
    void* user_ptr_to_block = (void*)((char*)all_init_memory_block_desc + sizeof(int) + sizeof(void*));
    memfree(user_ptr_to_block);
    EXPECT_EQ(*getleftsizeofblock(all_init_memory_block_desc), myabs(*getleftsizeofblock(all_init_memory_block_desc)));
    EXPECT_EQ(*getrightsizeofblock(all_init_memory_block_desc), myabs(*getrightsizeofblock(all_init_memory_block_desc)));
    free(ptr);
}

TEST(memfree_Test, memfree_memfreeFreeBlockWhereRightBlockIsFree_expectBlocksMerged) {
    const int TEST_MEMORY_SIZE_INIT = memgetminimumsize() + memgetblocksize() + 2 * sizeof(char);
    void* ptr = malloc(TEST_MEMORY_SIZE_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_INIT);
    assert(bytes_init);
    // first block is allocated
    void* allocated_char_block_desc = ptr;
    *getleftsizeofblock(allocated_char_block_desc) = -((int)sizeof(char) + memgetblocksize()); // block is allocated so size is negative
    *getblocknext(allocated_char_block_desc) = CNULL;
    *getrightsizeofblock(allocated_char_block_desc) = *getleftsizeofblock(allocated_char_block_desc);
    // second block is free
    void* free_char_block_desc = (void*)((char*)allocated_char_block_desc + myabs(*getleftsizeofblock(allocated_char_block_desc)));
    s_head = free_char_block_desc;
    *getleftsizeofblock(free_char_block_desc) = (int)sizeof(char) + memgetblocksize();
    *getblocknext(free_char_block_desc) = CNULL;
    *getrightsizeofblock(free_char_block_desc) = *getleftsizeofblock(free_char_block_desc);
    // now freeing and expect memory state similar with state after meminit
    void* user_ptr_to_allocated_block = (void*)((char*)allocated_char_block_desc + sizeof(int) + sizeof(void*));
    memfree(user_ptr_to_allocated_block);
    EXPECT_EQ(*getleftsizeofblock(allocated_char_block_desc), bytes_init);
    EXPECT_EQ(*getrightsizeofblock(allocated_char_block_desc), bytes_init);
    EXPECT_EQ(s_head, ptr);
    free(ptr);
}

TEST(memfree_Test, memfree_memfreeFreeBlockWhereLeftBlockIsFree_expectBlocksMerged) {
    const int TEST_MEMORY_SIZE_INIT = memgetminimumsize() + memgetblocksize() + 2 * sizeof(char);
    void* ptr = malloc(TEST_MEMORY_SIZE_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_INIT);
    assert(bytes_init);
    // first block is free
    void* free_char_block_desc = ptr;
    s_head = free_char_block_desc;
    *getleftsizeofblock(free_char_block_desc) = (int)sizeof(char) + memgetblocksize();
    *getblocknext(free_char_block_desc) = CNULL;
    *getrightsizeofblock(free_char_block_desc) = *getleftsizeofblock(free_char_block_desc);
    // second block is allocated
    void* allocated_char_block_desc = (void*)((char*)free_char_block_desc + myabs(*getleftsizeofblock(free_char_block_desc)));
    *getleftsizeofblock(allocated_char_block_desc) = -((int)sizeof(char) + memgetblocksize()); // block is allocated so size is negative
    *getblocknext(allocated_char_block_desc) = CNULL;
    *getrightsizeofblock(allocated_char_block_desc) = *getleftsizeofblock(allocated_char_block_desc);
    // now freeing and expect memory state similar with state after meminit
    void* user_ptr_to_allocated_block = (void*)((char*)allocated_char_block_desc + sizeof(int) + sizeof(void*));
    memfree(user_ptr_to_allocated_block);
    EXPECT_EQ(*getleftsizeofblock(free_char_block_desc), bytes_init);
    EXPECT_EQ(*getrightsizeofblock(free_char_block_desc), bytes_init);
    EXPECT_EQ(s_head, ptr);
    free(ptr);
}

TEST(memfree_Test, memfree_memfreeFreeBlockBetweenTwoFreeBlocks_expectBlocksMerged) {
    const int TEST_MEMORY_SIZE_INIT = memgetminimumsize() + 2 * memgetblocksize() + 3 * sizeof(char);
    void* ptr = malloc(TEST_MEMORY_SIZE_INIT);
    assert(ptr);
    int bytes_init = meminit(ptr, TEST_MEMORY_SIZE_INIT);
    assert(bytes_init);
    // first block is free
    void* first_free_char_block_desc = ptr;
    s_head = first_free_char_block_desc;
    *getleftsizeofblock(first_free_char_block_desc) = (int)sizeof(char) + memgetblocksize();
    *getblocknext(first_free_char_block_desc) = CNULL;
    *getrightsizeofblock(first_free_char_block_desc) = *getleftsizeofblock(first_free_char_block_desc);
    // second block is allocated
    void* allocated_char_block_desc = (void*)((char*)first_free_char_block_desc + myabs(*getleftsizeofblock(first_free_char_block_desc)));
    *getleftsizeofblock(allocated_char_block_desc) = -((int)sizeof(char) + memgetblocksize()); // block allocated so its size is negative
    *getblocknext(allocated_char_block_desc) = CNULL;
    *getrightsizeofblock(allocated_char_block_desc) = *getleftsizeofblock(allocated_char_block_desc);
    // third block is free
    void* third_free_char_block_desc = (void*)((char*)allocated_char_block_desc + myabs(*getleftsizeofblock(allocated_char_block_desc)));
    *getleftsizeofblock(third_free_char_block_desc) = (int)sizeof(char) + memgetblocksize();
    *getblocknext(third_free_char_block_desc) = CNULL;
    *getrightsizeofblock(third_free_char_block_desc) = *getleftsizeofblock(third_free_char_block_desc);
    *getblocknext(first_free_char_block_desc) = third_free_char_block_desc;
    // now freeing and expect memory state similar with state after meminit
    void* user_ptr_to_allocated_block = (void*)((char*)allocated_char_block_desc + sizeof(int) + sizeof(void*));
    memfree(user_ptr_to_allocated_block);
    EXPECT_EQ(*getleftsizeofblock(first_free_char_block_desc), bytes_init);
    EXPECT_EQ(*getrightsizeofblock(first_free_char_block_desc), bytes_init);
    EXPECT_EQ(s_head, ptr);
    free(ptr);
}
