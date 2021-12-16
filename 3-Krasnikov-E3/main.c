#include <stdio.h>

#define SOME_INIT_DATA 3886535

typedef struct Tree {
	int data;
	struct Tree* left;
	struct Tree* right;
} Tree;

typedef enum {
	NO,
	LEFT,
	RIGHT,
} NodePrefix;

static int _Min(int a, int b) {
	return a < b ? a : b;
}

void FillTreeWithMinLeafsHeight(Tree* tree) {
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

int main(void) {
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
}