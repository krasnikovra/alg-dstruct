#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define BUF_SIZE 16
#define INVALID_IDX -1
#define BTREE_PARAM 3
#define INT_MAX_LEN 10

typedef struct Node {
    bool isLeaf;
    int cnt;
    int* keys;
    struct Node** children; // children count is cnt + 1
} Node;

typedef struct {
    int t; // BTree param
    Node* root;
} BTree;

static Node* _NodeAllocate(const int t);
static void _NodeDestroy(Node* node);
static int _FindKeyInArr(const int* arr, const int size, const int key);
static bool _InsertNonfull(Node* node, const int key, const int t);
static bool _SplitChild(Node* node, const int t, const int i);
static Node* _FindNodeContainsKey(const BTree* tree, const int key, int* idx);
static int _GetPredecessor(Node* node, const int idx);
static int _GetSuccessor(Node* node, const int idx);
static void _RemoveFromArrI(int* arr, const int size, const int idx);
static void _RemoveFromArrNP(Node** arr, const int size, const int idx);
static void _LeftRotate(Node* node, const int keyIdx);
static void _RightRotate(Node* node, const int keyIdx);
static void _MergeChild(Node* node, const int keyIdx);
static void _RemoveFromLeaf(Node* node, const int t, const int key);
static void _Remove(Node* node, Node* nodeContainsKey, const int idx, const int t, const int key);
static void _Destroy(Node* node);
static void _Print(const Node* node, const int t, int margin);

bool BTreeInit(BTree* tree);
bool BTreeFind(const BTree* tree, const int key);
bool BTreeInsert(BTree* tree, const int key);
void BTreeRemove(BTree* tree, const int key);
void BTreeDestroy(BTree* tree);
void BTreePrint(BTree* tree);

int TestSystemMainCycle(FILE* in, FILE* out);

int main() {
    return TestSystemMainCycle(stdin, stdout);
}

Node* _NodeAllocate(const int t) {
    Node* res = (Node*)malloc(sizeof(Node));
    if (!res)
        return NULL;
    res->keys = (int*)malloc((2 * t - 1) * sizeof(int));
    if (!res->keys) {
        free(res);
        return NULL;
    }
    res->children = (Node**)malloc(2 * t * sizeof(Node*));
    if (!res->children) {
        free(res->keys);
        free(res);
        return NULL;
    }
    res->cnt = 0;
    res->isLeaf = false;
    for (int i = 0; i < 2 * t; ++i)
        res->children[i] = NULL;
    return res;
}

void _NodeDestroy(Node* node) {
    free(node->children);
    free(node->keys);
    free(node);
}

int _FindKeyInArr(const int* arr, const int size, const int key) {
    for (int i = 0; i < size; ++i)
        if (arr[i] == key)
            return i;
    return INVALID_IDX;
}

bool BTreeInit(BTree* tree) {
    Node* root = _NodeAllocate(tree->t);
    if (!root)
        return false;
    root->cnt = 0;
    root->isLeaf = true;
    tree->root = root;
    return true;
}

bool BTreeFind(const BTree* tree, const int key) {
    int idx = INVALID_IDX;
    Node* found = _FindNodeContainsKey(tree, key, &idx);
    return found;
}

bool BTreeInsert(BTree* tree, const int key) {
    if (BTreeFind(tree, key))
        return false;

    Node* oldRoot = tree->root;
    // need node split
    if (oldRoot->cnt == 2 * tree->t - 1) {
        Node* newRoot = _NodeAllocate(tree->t);
        if (!newRoot)
            return false;
        tree->root = newRoot;
        newRoot->isLeaf = false;
        newRoot->cnt = 0;
        newRoot->children[0] = oldRoot;
        if (!_SplitChild(newRoot, tree->t, 0)) {
            _NodeDestroy(newRoot);
            tree->root = oldRoot;
            return false;
        }
        return _InsertNonfull(newRoot, key, tree->t);
    }
    // simple nonfull insert
    return _InsertNonfull(oldRoot, key, tree->t);
}

bool _InsertNonfull(Node* node, const int key, const int t) {
    // node should not be full
    assert(node->cnt < 2 * t - 1);

    int i = node->cnt - 1;
    if (node->isLeaf) {
        // shifting bigger-than-key part to the right 
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            --i;
        }
        // and inserting key
        node->keys[i + 1] = key;
        ++node->cnt;
        return true;
    }
    else {
        // looking for the appropriate child
        while (i >= 0 && key < node->keys[i]) {
            --i;
        }
        ++i;
        // if appropriate child is full we need to split it
        if (node->children[i]->cnt == 2 * t - 1) {
            if (!_SplitChild(node, t, i))
                return false;
            if (key > node->keys[i])
                ++i;
        }
        // now insert in the appropriate child
        return _InsertNonfull(node->children[i], key, t);
    }
}

bool _SplitChild(Node* node, const int t, const int i) {
    assert(node->children[i]->cnt == 2 * t - 1);

    Node* newNode = _NodeAllocate(t);
    if (!newNode)
        return false;

    Node* child = node->children[i];
    newNode->isLeaf = child->isLeaf; // they will be the same level
    newNode->cnt = t - 1;
    for (int j = 0; j < t - 1; ++j)
        newNode->keys[j] = child->keys[j + t];
    if (!child->isLeaf) // need to copy childrens only if not leaf
        for (int j = 0; j < t; ++j)
            newNode->children[j] = child->children[j + t];
    child->cnt = t - 1;
    // now newNode and child are prepared to be attached to the node
    for (int j = node->cnt + 1; j > i + 1; --j)
        node->children[j] = node->children[j - 1];
    node->children[i + 1] = newNode;
    for (int j = node->cnt; j > i; --j)
        node->keys[j] = node->keys[j - 1];
    node->keys[i] = child->keys[t - 1];
    ++node->cnt;
    node->isLeaf = false;
    return true;
}

Node* _FindNodeContainsKey(const BTree* tree, const int key, int* idx) {
    Node* iter = tree->root;
    while (!iter->isLeaf) {
        *idx = _FindKeyInArr(iter->keys, iter->cnt, key);
        if (*idx != INVALID_IDX) 
            return iter;
        // if greater than every child
        // proceed to the last child
        if (key >= iter->keys[iter->cnt - 1]) {
            iter = iter->children[iter->cnt];
            continue;
        }
        // or we find next Node manually
        for (int i = 0; i < iter->cnt; ++i)
            if (key < iter->keys[i]) {
                iter = iter->children[i];
                break;
            }
    }
    // here we have iter as a leaf so key is here or nowhere
    *idx = _FindKeyInArr(iter->keys, iter->cnt, key);
    return *idx != INVALID_IDX ? iter : NULL;
}

int _GetPredecessor(Node* node, const int idx) {
    node = node->children[idx];
    while (!node->isLeaf)
        node = node->children[node->cnt];
    return node->keys[node->cnt - 1];
}

int _GetSuccessor(Node* node, const int idx) {
    node = node->children[idx + 1];
    while (!node->isLeaf)
        node = node->children[0];
    return node->keys[0];
}

void _RemoveFromArrI(int* arr, const int size, const int idx) {
    for (int i = idx; i + 1 < size; ++i)
        arr[i] = arr[i + 1];
}

void _RemoveFromArrNP(Node** arr, const int size, const int idx) {
    for (int i = idx; i + 1 < size; ++i)
        arr[i] = arr[i + 1];
}

void _LeftRotate(Node* node, const int keyIdx) {
    Node* lNeighbour = node->children[keyIdx];
    Node* rNeighbour = node->children[keyIdx + 1];
    ++lNeighbour->cnt;
    lNeighbour->keys[lNeighbour->cnt - 1] = node->keys[keyIdx];
    lNeighbour->children[lNeighbour->cnt] = rNeighbour->children[0];
    node->keys[keyIdx] = rNeighbour->keys[0];
    for (int i = 0; i + 1 < rNeighbour->cnt; ++i)
        rNeighbour->keys[i] = rNeighbour->keys[i + 1];
    for (int i = 0; i + 1 < rNeighbour->cnt + 1; ++i)
        rNeighbour->children[i] = rNeighbour->children[i + 1];
    --rNeighbour->cnt;
}

void _RightRotate(Node* node, const int keyIdx) {
    Node* lNeighbour = node->children[keyIdx];
    Node* rNeighbour = node->children[keyIdx + 1];
    ++rNeighbour->cnt;
    for (int i = rNeighbour->cnt - 1; i - 1 >= 0; --i)
        rNeighbour->keys[i] = rNeighbour->keys[i - 1];
    rNeighbour->keys[0] = node->keys[keyIdx];
    for (int i = rNeighbour->cnt; i - 1 >= 0; --i)
        rNeighbour->children[i] = rNeighbour->children[i - 1];
    rNeighbour->children[0] = lNeighbour->children[lNeighbour->cnt];
    node->keys[keyIdx] = lNeighbour->keys[lNeighbour->cnt - 1];
    --lNeighbour->cnt;
}

void _MergeChild(Node* node, const int keyIdx) {
    Node* lChild = node->children[keyIdx];
    Node* rChild = node->children[keyIdx + 1];
    //collecting new node (lchild merged with node[keyIdx] + rChild)
    lChild->keys[lChild->cnt] = node->keys[keyIdx];
    for (int i = 0; i < rChild->cnt; ++i)
        lChild->keys[lChild->cnt + 1 + i] = rChild->keys[i];
    for (int i = 0; i < rChild->cnt + 1; ++i)
        lChild->children[lChild->cnt + 1 + i] = rChild->children[i];
    lChild->cnt += 1 + rChild->cnt;
    _RemoveFromArrI(node->keys, node->cnt, keyIdx);
    _RemoveFromArrNP(node->children, node->cnt + 1, keyIdx + 1);
    --node->cnt;
    _NodeDestroy(rChild);
}

void _RemoveFromLeaf(Node* node, const int t, const int key) {
    // find appropriate child
    int i = node->cnt - 1;
    while (i >= 0 && key < node->keys[i]) {
        --i;
    }
    ++i;
    Node* child = node->children[i];
    int idx = _FindKeyInArr(child->keys, child->cnt, key);
    if (idx != INVALID_IDX) {
        assert(child->isLeaf);
        _RemoveFromArrI(child->keys, child->cnt, idx);
        --child->cnt;
    }
    else {
        if (child->isLeaf)
            return;
        _RemoveFromLeaf(child, t, key);
    }
    // fixing antioverflows from bottom to top
    // with the recursion reverse stroke
    if (child->cnt < t - 1) {
        // we have antioverflow
        if (i < node->cnt) {
            // we have right neighbour of child
            Node* rNeighbour = node->children[i + 1];
            if (child->cnt + rNeighbour->cnt + 1 <= 2 * t - 1) {
                // we can merge them which is priority case because
                // one Node will be destroyed after this operation
                _MergeChild(node, i);
            }
            else
                // we can rotate to the left to remanage values in Nodes
                _LeftRotate(node, i);
        }
        else {
            // only left neighbour of child 
            Node* lNeighbour = node->children[i - 1];
            if (child->cnt + lNeighbour->cnt + 1 <= 2 * t - 1) {
                // we can merge them which is priority case because
                // one Node will be destroyed after this operation
                _MergeChild(node, i - 1);
            }
            else
                // we can rotate to the right to remanage values in Nodes
                _RightRotate(node, i - 1);
        }
    }
}

void _Remove(Node* node, Node* nodeContainsKey, const int idx, const int t, const int key) {
    if (!nodeContainsKey)
        // do nothing if no such key
        return;
    if (nodeContainsKey->isLeaf)
        // _RemoveFromLeaf is fine
        _RemoveFromLeaf(node, t, key);
    else {
        if (nodeContainsKey->children[idx]->cnt >= t) {
            int pred = _GetPredecessor(nodeContainsKey, idx);
            _RemoveFromLeaf(node, t, pred);
            nodeContainsKey->keys[idx] = pred;
        }
        else if (nodeContainsKey->children[idx + 1]->cnt >= t) {
            int succ = _GetSuccessor(nodeContainsKey, idx);
            _RemoveFromLeaf(node, t, succ);
            nodeContainsKey->keys[idx] = succ;
        }
        else {
            _MergeChild(nodeContainsKey, idx);
            _Remove(node, nodeContainsKey->children[idx], (2 * t - 1) / 2, t, key);
        }
    }
}

void BTreeRemove(BTree* tree, const int key) {
    if (tree->root->isLeaf) {
        int idx = _FindKeyInArr(tree->root->keys, tree->root->cnt, key);
        if (idx != INVALID_IDX) {
            _RemoveFromArrI(tree->root->keys, tree->root->cnt, idx);
            --tree->root->cnt;
            // do nothing if root->cnt is 0 as its the whole tree
            // if BTreeDestroy here user will need to re-init tree
            // which is anomaly
        }
        // do nothing if no such key
    }
    else {
        int idx = INVALID_IDX;
        Node* neededNode = _FindNodeContainsKey(tree, key, &idx);
        _Remove(tree->root, neededNode, idx, tree->t, key);
        if (tree->root->cnt == 0) {
            // after recursion reverse stroke root could become empty
            Node* root = tree->root;
            tree->root = tree->root->children[0];
            _NodeDestroy(root);
        }
    }
}

void _Destroy(Node* node) {
    if (!node->isLeaf)
        for (int i = 0; i < node->cnt + 1; ++i)
            _Destroy(node->children[i]);
    _NodeDestroy(node);
}

void BTreeDestroy(BTree* tree) {
    _Destroy(tree->root);
    tree->root = NULL;
}

int TestSystemMainCycle(FILE* in, FILE* out) {
    BTree tree = { BTREE_PARAM, NULL };
    BTreeInit(&tree);
    char buf[BUF_SIZE] = { 0 };
    char c = 0;
    int val = 0;
    while (fgets(buf, BUF_SIZE, in)) {
        sscanf(buf, "%c %d", &c, &val);
        switch (c) {
        case 'a':
            BTreeInsert(&tree, val);
            break;
        case 'r':
            BTreeRemove(&tree, val);
            break;
        case 'f':
            fprintf(out, "%s\n", BTreeFind(&tree, val) ? "yes" : "no");
            break;
        default:
            BTreeDestroy(&tree);
            return 0;
        }
    }
    BTreeDestroy(&tree);
    return 0;
}

void _Print(const Node* node, const int t, int margin) {
    if (!node->isLeaf)
        for (int i = 0; i < (node->cnt + 1) / 2; i++)
            _Print(node->children[i], t, margin + 1);
    for (int i = 0; i < margin * ((INT_MAX_LEN + 1) * (2 * t - 1) + 1); i++)
        // (INT_MAX_LEN + 2) * (2 * t - 1) is how much ints take + comma (and last bracket)
        // + 1 is the first bracket
        printf(" ");
    printf("[");
    for (int i = 0; i < node->cnt - 1; ++i)
        printf("%i,", node->keys[i]);
    printf("%i]\n", node->keys[node->cnt - 1]);
    if (!node->isLeaf)
        for (int i = (node->cnt + 1) / 2; i < node->cnt + 1; i++)
            _Print(node->children[i], t, margin + 1);
}

/*
                                                                    [2,4,5]
                                  [9]
                                                                    [12,13,14]
[15]
                                                                    [17,25]
                                                                    [32]
                                  [27,33,39]
                                                                    [35]
                                                                    [47,54,65]
*/
void BTreePrint(BTree* tree) {
    _Print(tree->root, tree->t, 0);
}