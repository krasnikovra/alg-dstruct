#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <gtest/gtest.h>
#include <stdio.h>
#include <stdbool.h>
#include "tree.h"

#define SOME_INIT_DATA 28164
#define SPACES 1
#define BUF_SIZE 1024
#define CORRECT_FILENAME(TestCase, TestName) #TestCase"/"#TestName".txt"
#define OUTPUT_FILENAME(TestCase, TestName) #TestCase"/"#TestName"_output.txt" // is being used in .gitignore

class FunctionalTest : public ::testing::Test {
protected:
    void SetUp() {
        _CrtMemCheckpoint(&m_before);
    }
    void TearDown() {
        _CrtMemCheckpoint(&m_after);
        if (_CrtMemDifference(&m_diff, &m_before, &m_after)) {
            _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
            _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
            _CrtMemDumpStatistics(&m_diff);
            FAIL();
        }
    }
private:
    _CrtMemState m_before, m_after, m_diff;
};

class FillTreeFuncTest : public FunctionalTest {};
class PrintTreeFuncTest : public FunctionalTest {};

void InitNodesArray(Tree* nodes, int size, int data) {
    for (int i = 0; i < size; i++) {
        nodes[i].data = data;
        nodes[i].left = nodes[i].right = nullptr;
    }
}

TEST_F(FillTreeFuncTest, OneNodeTree) {
    /*
        0
    */

    Tree tree = { SOME_INIT_DATA, nullptr };
    FillTreeWithMinLeafsHeight(&tree);
    EXPECT_EQ(tree.data, 0); // leaf's min leafs height in subtrees is assumed to be zero
}

TEST_F(FillTreeFuncTest, OneLevelTree) {
    /*
          0
         / \
        1   2
    */

    Tree nodes[3] = { 0 };
    const int size = sizeof(nodes) / sizeof(Tree);
    InitNodesArray(nodes, size, SOME_INIT_DATA);

    nodes[0].left = &nodes[1];
    nodes[0].right = &nodes[2];

    FillTreeWithMinLeafsHeight(&nodes[0]);

    EXPECT_EQ(nodes[0].data, 1);
    EXPECT_EQ(nodes[1].data, 0);
    EXPECT_EQ(nodes[2].data, 0);
}

TEST_F(FillTreeFuncTest, OneWayTree) {
    /*
            0
           /
          1
           \
            2
             \
              3
             /
            4
    */

    Tree nodes[5] = { 0 };
    const int size = sizeof(nodes) / sizeof(Tree);
    InitNodesArray(nodes, size, SOME_INIT_DATA);

    nodes[0].left = &nodes[1];
    nodes[1].right = &nodes[2];
    nodes[2].right = &nodes[3];
    nodes[3].left = &nodes[4];

    FillTreeWithMinLeafsHeight(&nodes[0]);

    EXPECT_EQ(nodes[0].data, 4);
    EXPECT_EQ(nodes[1].data, 3);
    EXPECT_EQ(nodes[2].data, 2);
    EXPECT_EQ(nodes[3].data, 1);
    EXPECT_EQ(nodes[4].data, 0);
}

TEST_F(FillTreeFuncTest, NearlyOneWayTree) {
    /*
        0
       /
      1
       \
        2
       / \
      5   3
         /
        4
    */

    Tree nodes[6] = { 0 };
    const int size = sizeof(nodes) / sizeof(Tree);
    InitNodesArray(nodes, size, SOME_INIT_DATA);

    nodes[0].left = &nodes[1];
    nodes[1].right = &nodes[2];
    nodes[2].right = &nodes[3];
    nodes[2].left = &nodes[5];
    nodes[3].left = &nodes[4];

    FillTreeWithMinLeafsHeight(&nodes[0]);

    EXPECT_EQ(nodes[0].data, 3); // here and so on to 2nd node 5th node
    EXPECT_EQ(nodes[1].data, 2); // makes nodes 0, 1, 2 have its data less by 1
    EXPECT_EQ(nodes[2].data, 1); // compared with previous test OneWayTree
    EXPECT_EQ(nodes[3].data, 1);
    EXPECT_EQ(nodes[4].data, 0);
    EXPECT_EQ(nodes[5].data, 0);
}

TEST_F(FillTreeFuncTest, ComplicatedTree) {
    /*
             0
            / \
           1   2
              / \
             3   4
            / \   \
           5   6   7
                  /
                 8
    */

    Tree nodes[9] = { 0 };
    const int size = sizeof(nodes) / sizeof(Tree);
    InitNodesArray(nodes, size, SOME_INIT_DATA);

    nodes[0].left = &nodes[1];
    nodes[0].right = &nodes[2];
    nodes[2].left = &nodes[3];
    nodes[2].right = &nodes[4];
    nodes[3].left = &nodes[5];
    nodes[3].right = &nodes[6];
    nodes[4].right = &nodes[7];
    nodes[7].left = &nodes[8];

    FillTreeWithMinLeafsHeight(&nodes[0]);

    EXPECT_EQ(nodes[0].data, 1);
    EXPECT_EQ(nodes[1].data, 0);
    EXPECT_EQ(nodes[2].data, 2);
    EXPECT_EQ(nodes[3].data, 1);
    EXPECT_EQ(nodes[4].data, 2);
    EXPECT_EQ(nodes[5].data, 0);
    EXPECT_EQ(nodes[6].data, 0);
    EXPECT_EQ(nodes[7].data, 1);
    EXPECT_EQ(nodes[8].data, 0);
}

bool FileCmp(const char* filename1, const char* filename2) {
    FILE* file1 = fopen(filename1, "rb");
    if (!file1)
        return false;
    FILE* file2 = fopen(filename2, "rb");
    if (!file2) {
        fclose(file1);
        return false;
    }
    char buf1[BUF_SIZE] = { 0 }, buf2[BUF_SIZE] = { 0 };
    fread(buf1, sizeof(char), BUF_SIZE, file1);
    fread(buf2, sizeof(char), BUF_SIZE, file2);
    fclose(file1);
    fclose(file2);
    return !strcmp(buf1, buf2);
}

void PrintTreeFuncTestSuite(Tree* tree, const char* filenameOut, const char* filenameCorrect) {
    FILE* fileOut = fopen(filenameOut, "w");
    if (!fileOut)
        ASSERT_TRUE(fileOut);
    PrintTree(fileOut, tree, SPACES);
    fclose(fileOut);
    ASSERT_TRUE(FileCmp(filenameOut, filenameCorrect));
}

void InitNodesArrayWithIndices(Tree* nodes, int size) {
    for (int i = 0; i < size; i++) {
        nodes[i].data = i;
        nodes[i].left = nodes[i].right = nullptr;
    }
}

TEST_F(PrintTreeFuncTest, OneNodeTree) {
    /*
        0
    */

    Tree tree = { 0, nullptr };
    PrintTreeFuncTestSuite(&tree, OUTPUT_FILENAME(PrintTreeFuncTest, OneNodeTree), CORRECT_FILENAME(PrintTreeFuncTest, OneNodeTree));
}

TEST_F(PrintTreeFuncTest, OneLevelTree) {
    /*
          0
         / \
        1   2
    */

    Tree nodes[3] = { 0 };
    const int size = sizeof(nodes) / sizeof(Tree);
    InitNodesArrayWithIndices(nodes, size);

    nodes[0].left = &nodes[1];
    nodes[0].right = &nodes[2];

    PrintTreeFuncTestSuite(&nodes[0], OUTPUT_FILENAME(PrintTreeFuncTest, OneLevelTree), CORRECT_FILENAME(PrintTreeFuncTest, OneLevelTree));
}

TEST_F(PrintTreeFuncTest, OneWayTree) {
    /*
            0
           /
          1
           \
            2
             \
              3
             /
            4
    */

    Tree nodes[5] = { 0 };
    const int size = sizeof(nodes) / sizeof(Tree);
    InitNodesArrayWithIndices(nodes, size);

    nodes[0].left = &nodes[1];
    nodes[1].right = &nodes[2];
    nodes[2].right = &nodes[3];
    nodes[3].left = &nodes[4];

    PrintTreeFuncTestSuite(&nodes[0], OUTPUT_FILENAME(PrintTreeFuncTest, OneWayTree), CORRECT_FILENAME(PrintTreeFuncTest, OneWayTree));
}

TEST_F(PrintTreeFuncTest, NearlyOneWayTree) {
    /*
        0
       /
      1
       \
        2
       / \
      5   3
         /
        4
    */

    Tree nodes[6] = { 0 };
    const int size = sizeof(nodes) / sizeof(Tree);
    InitNodesArrayWithIndices(nodes, size);

    nodes[0].left = &nodes[1];
    nodes[1].right = &nodes[2];
    nodes[2].right = &nodes[3];
    nodes[2].left = &nodes[5];
    nodes[3].left = &nodes[4];

    PrintTreeFuncTestSuite(&nodes[0], OUTPUT_FILENAME(PrintTreeFuncTest, NearlyOneWayTree), CORRECT_FILENAME(PrintTreeFuncTest, NearlyOneWayTree));
}

TEST_F(PrintTreeFuncTest, ComplicatedTree) {
    /*
             0
            / \
           1   2
              / \
             3   4
            / \   \
           5   6   7
                  /
                 8
    */

    Tree nodes[9] = { 0 };
    const int size = sizeof(nodes) / sizeof(Tree);
    InitNodesArrayWithIndices(nodes, size);

    nodes[0].left = &nodes[1];
    nodes[0].right = &nodes[2];
    nodes[2].left = &nodes[3];
    nodes[2].right = &nodes[4];
    nodes[3].left = &nodes[5];
    nodes[3].right = &nodes[6];
    nodes[4].right = &nodes[7];
    nodes[7].left = &nodes[8];

    PrintTreeFuncTestSuite(&nodes[0], OUTPUT_FILENAME(PrintTreeFuncTest, ComplicatedTree), CORRECT_FILENAME(PrintTreeFuncTest, ComplicatedTree));
}

//-----------------------------------------------------
// Functional Tests
//-----------------------------------------------------

auto& FunctionalTestSuite = PrintTreeFuncTestSuite;

TEST_F(FunctionalTest, OneNodeTree) {
    /*
        0
    */

    Tree tree = { 0, nullptr };
    FillTreeWithMinLeafsHeight(&tree);
    FunctionalTestSuite(&tree, OUTPUT_FILENAME(FunctionalTest, OneNodeTree), CORRECT_FILENAME(FunctionalTest, OneNodeTree));
}

TEST_F(FunctionalTest, OneLevelTree) {
    /*
          0
         / \
        1   2
    */

    Tree nodes[3] = { 0 };
    const int size = sizeof(nodes) / sizeof(Tree);
    InitNodesArray(nodes, size, SOME_INIT_DATA);

    nodes[0].left = &nodes[1];
    nodes[0].right = &nodes[2];

    FillTreeWithMinLeafsHeight(&nodes[0]);
    FunctionalTestSuite(&nodes[0], OUTPUT_FILENAME(FunctionalTest, OneLevelTree), CORRECT_FILENAME(FunctionalTest, OneLevelTree));
}

TEST_F(FunctionalTest, OneWayTree) {
    /*
            0
           /
          1
           \
            2
             \
              3
             /
            4
    */

    Tree nodes[5] = { 0 };
    const int size = sizeof(nodes) / sizeof(Tree);
    InitNodesArray(nodes, size, SOME_INIT_DATA);

    nodes[0].left = &nodes[1];
    nodes[1].right = &nodes[2];
    nodes[2].right = &nodes[3];
    nodes[3].left = &nodes[4];

    FillTreeWithMinLeafsHeight(&nodes[0]);
    FunctionalTestSuite(&nodes[0], OUTPUT_FILENAME(FunctionalTest, OneWayTree), CORRECT_FILENAME(FunctionalTest, OneWayTree));
}

TEST_F(FunctionalTest, NearlyOneWayTree) {
    /*
        0
       /
      1
       \
        2
       / \
      5   3
         /
        4
    */

    Tree nodes[6] = { 0 };
    const int size = sizeof(nodes) / sizeof(Tree);
    InitNodesArray(nodes, size, SOME_INIT_DATA);

    nodes[0].left = &nodes[1];
    nodes[1].right = &nodes[2];
    nodes[2].right = &nodes[3];
    nodes[2].left = &nodes[5];
    nodes[3].left = &nodes[4];

    FillTreeWithMinLeafsHeight(&nodes[0]);
    FunctionalTestSuite(&nodes[0], OUTPUT_FILENAME(FunctionalTest, NearlyOneWayTree), CORRECT_FILENAME(FunctionalTest, NearlyOneWayTree));
}

TEST_F(FunctionalTest, ComplicatedTree) {
    /*
             0
            / \
           1   2
              / \
             3   4
            / \   \
           5   6   7
                  /
                 8
    */

    Tree nodes[9] = { 0 };
    const int size = sizeof(nodes) / sizeof(Tree);
    InitNodesArray(nodes, size, SOME_INIT_DATA);

    nodes[0].left = &nodes[1];
    nodes[0].right = &nodes[2];
    nodes[2].left = &nodes[3];
    nodes[2].right = &nodes[4];
    nodes[3].left = &nodes[5];
    nodes[3].right = &nodes[6];
    nodes[4].right = &nodes[7];
    nodes[7].left = &nodes[8];

    FillTreeWithMinLeafsHeight(&nodes[0]);
    FunctionalTestSuite(&nodes[0], OUTPUT_FILENAME(FunctionalTest, ComplicatedTree), CORRECT_FILENAME(FunctionalTest, ComplicatedTree));
}