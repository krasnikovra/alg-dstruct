#define _CRT_SECURE_NO_WARNINGS
#define TEST_SYSTEM_ENTRY
#define BUF_SIZE 16

#include <stdio.h>
#include <stdlib.h>

typedef struct Treap {
    int tKey; // treap is a bin tree by this key
    int hKey; // an a bin heap by this key
    struct Treap* left;
    struct Treap* right;
} Treap;

typedef struct {
    Treap* t1;
    Treap* t2;
} TreapsPair;

TreapsPair TreapSplit(Treap* treap, int tKey);
Treap* TreapMerge(TreapsPair treapSplit);
Treap* TreapFind(Treap* treap, int tKey);
Treap* TreapInsert(Treap* treap, int tKey, int hKey);
Treap* TreapRemove(Treap* treap, int tKey);
void TreapDestroy(Treap* treap);
int TreapSumOfHKeys(Treap* treap);
int TreapSumOfHKeysLimit(Treap* treap, int ltKey, int rtKey); // sum of all y in (x,y) such as ltKey <= y < rKey

int TestSystemMainCycle(FILE* in, FILE* out);

int main(void) {
#ifdef TEST_SYSTEM_ENTRY
    return TestSystemMainCycle(stdin, stdout);
#else
    // test treap
    Treap* treap = NULL;
    treap = TreapInsert(treap, 1, 10);
    treap = TreapInsert(treap, 2, 40);
    treap = TreapInsert(treap, 3, 30);
    treap = TreapInsert(treap, 4, 70);
    treap = TreapInsert(treap, 5, 20);

    int res = TreapSumOfHKeysLimit(treap, 2, 5); // expect res == 140 (40 + 30 + 70)

    return 0;
#endif
}

TreapsPair TreapSplit(Treap* treap, int tKey) {
    if (!treap)
        return (TreapsPair) { NULL, NULL };
    if (tKey > treap->tKey) {
        TreapsPair treapSplit = TreapSplit(treap->right, tKey);
        treap->right = treapSplit.t1;
        return (TreapsPair) { treap, treapSplit.t2 };
    }
    else {
        TreapsPair treapSplit = TreapSplit(treap->left, tKey);
        treap->left = treapSplit.t2;
        return (TreapsPair) { treapSplit.t1, treap };
    }
}

Treap* TreapMerge(TreapsPair treapSplit) {
    if (!treapSplit.t1)
        return treapSplit.t2;
    if (!treapSplit.t2)
        return treapSplit.t1;
    if (treapSplit.t1->hKey < treapSplit.t2->hKey) {
        treapSplit.t1->right = TreapMerge((TreapsPair) { treapSplit.t1->right, treapSplit.t2 });
        return treapSplit.t1;
    }
    else {
        treapSplit.t2->left = TreapMerge((TreapsPair) { treapSplit.t1, treapSplit.t2->left });
        return treapSplit.t2;
    }
}

Treap* TreapFind(Treap* treap, int tKey) {
    if (!treap)
        return NULL;
    if (tKey == treap->tKey)
        return treap;
    if (tKey > treap->tKey)
        return TreapFind(treap->right, tKey);
    else
        return TreapFind(treap->left, tKey);
}

Treap* TreapInsert(Treap* treap, int tKey, int hKey) {
    if (TreapFind(treap, tKey))
        return treap;
    
    Treap* newNode = (Treap*)malloc(sizeof(Treap));
    if (!newNode)
        return treap;
    newNode->left = newNode->right = NULL;
    newNode->tKey = tKey;
    newNode->hKey = hKey;

    TreapsPair treapSplit = TreapSplit(treap, tKey);
    treapSplit.t1 = TreapMerge((TreapsPair) { treapSplit.t1, newNode });
    return TreapMerge(treapSplit);
}

Treap* TreapRemove(Treap* treap, int tKey) {
    TreapsPair treapSplit = TreapSplit(treap, tKey);
    TreapsPair T2WithRemovingElem = TreapSplit(treapSplit.t2, tKey + 1);
    if (T2WithRemovingElem.t1)
        free(T2WithRemovingElem.t1);
    treap = TreapMerge((TreapsPair) { treapSplit.t1, T2WithRemovingElem.t2 });
    return treap;
}

void TreapDestroy(Treap* treap) {
    if (treap) {
        TreapDestroy(treap->left);
        TreapDestroy(treap->right);
        free(treap);
    }
}

int TreapSumOfHKeys(Treap* treap) {
    if (!treap)
        return 0;
    return treap->hKey + TreapSumOfHKeys(treap->left) + TreapSumOfHKeys(treap->right);
}

int TreapSumOfHKeysLimit(Treap* treap, int ltKey, int rtKey) {
    TreapsPair TreapSplitByLKey = TreapSplit(treap, ltKey);
    TreapsPair T2SplitByRKey = TreapSplit(TreapSplitByLKey.t2, rtKey);
    int res = TreapSumOfHKeys(T2SplitByRKey.t1);
    TreapSplitByLKey.t2 = TreapMerge(T2SplitByRKey);
    treap = TreapMerge(TreapSplitByLKey);
    return res;
}

int TestSystemMainCycle(FILE* in, FILE* out) {
    Treap* treap = NULL;
    char buf[BUF_SIZE] = { 0 };
    char c = 0;
    int val = 0;
    while (fgets(buf, BUF_SIZE, in)) {
        sscanf(buf, "%c %d", &c, &val);
        switch (c) {
        case 'a':
            treap = TreapInsert(treap, val, rand());
            break;
        case 'r':
            treap = TreapRemove(treap, val);
            break;
        case 'f':
            fprintf(out, "%s\n", TreapFind(treap, val) ? "yes" : "no");
            break;
        default:
            return -1;
        }
    }
    TreapDestroy(treap);
    return 0;
}