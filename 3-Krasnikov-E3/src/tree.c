#include <stdio.h>
#include "tree.h"

typedef enum {
    NO,
    LEFT,
    RIGHT,
} NodePrefix;

static int _Min(int a, int b) {
    return a < b ? a : b;
}

void FillTreeWithMinLeafsHeight(Tree* tree) {
    if (!tree)
        return;
    if (!tree->left) {
        if (!tree->right) {
            tree->data = 0;
            return;
        }
        FillTreeWithMinLeafsHeight(tree->right);
        tree->data = 1 + tree->right->data;
        return;
    }
    if (!tree->right) {
        FillTreeWithMinLeafsHeight(tree->left);
        tree->data = 1 + tree->left->data;
        return;
    }
    FillTreeWithMinLeafsHeight(tree->left);
    FillTreeWithMinLeafsHeight(tree->right);
    tree->data = 1 + _Min(tree->left->data, tree->right->data);
}

static void _PrintTree(FILE* stream, Tree* tree, int margin, int spaces, NodePrefix slashOritentation) {
    if (tree) {
        _PrintTree(stream, tree->right, margin + 1, spaces, (NodePrefix)RIGHT);
        for (int i = 0; i < margin * (spaces + 1); i++)
            fprintf(stream, " ");
        switch (slashOritentation) {
        case NO:
            fprintf(stream, " ");
            break;
        case LEFT:
            fprintf(stream, "\\");
            break;
        case RIGHT:
            fprintf(stream, "/");
            break;
        }
        fprintf(stream, "%d\n", tree->data);
        _PrintTree(stream, tree->left, margin + 1, spaces, (NodePrefix)LEFT);
    }
}

void PrintTree(FILE* stream, Tree* tree, int spaces) {
    _PrintTree(stream, tree, 0, spaces, (NodePrefix)NO);
}
