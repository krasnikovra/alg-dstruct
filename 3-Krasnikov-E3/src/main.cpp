#include <stdio.h>
#include <gtest/gtest.h>
#include "tree.h"

int main(int argc, char** argv) {
#ifdef _DEBUG
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
#else
    //some tree
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
    nodes[0].left = &nodes[1];
    nodes[0].right = &nodes[2];
    nodes[2].left = &nodes[3];
    nodes[2].right = &nodes[4];
    nodes[3].left = &nodes[5];
    nodes[3].right = &nodes[6];
    nodes[4].right = &nodes[7];
    nodes[7].left = &nodes[8];

    FillTreeWithMinLeafsHeight(&nodes[0]);

    printf("Tree:\n");
    PrintTree(stdout, &nodes[0], 1);

    return 0;
#endif
}
