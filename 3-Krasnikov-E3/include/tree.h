#ifndef TREE_H
#define TREE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Tree {
    int data;
    struct Tree* left;
    struct Tree* right;
} Tree;

void FillTreeWithMinLeafsHeight(Tree* tree);
void PrintTree(FILE* stream, Tree* tree, int spaces);

#ifdef __cplusplus
}
#endif

#endif
