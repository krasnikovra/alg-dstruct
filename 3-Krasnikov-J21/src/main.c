#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define HASH_MAP_SIZE 5000101 // prime
#define HASH_MAP_STEP 7 // prime
#define BUF_SIZE 128

typedef enum {
    WAS_NOT_USED,
    FREE,
    BUSY
} Status;

typedef struct {
    char* str;
    Status status;
} Cell;

typedef struct {
    Cell* data;
    size_t size;
    size_t step;
} HashMap;

static size_t _FindIndexByStr(const HashMap* hashMap, const char* str);

size_t Hashify(const char* str, const size_t hashMapSize);
HashMap HashMapInit(const size_t size, const size_t step);
bool HashMapInsert(HashMap* hashMap, const char* str);
bool HashMapFind(const HashMap* hashMap, const char* str);
void HashMapRemove(HashMap* hashMap, const char* str);
void HashMapDestroy(HashMap* hashMap);

int TestSystemMainCycle(FILE* in, FILE* out);

int main() {
    return TestSystemMainCycle(stdin, stdout);
}

static size_t _FindIndexByStr(const HashMap* hashMap, const char* str) {
    size_t index = Hashify(str, hashMap->size);
    const size_t indexStart = index;
    while (!hashMap->data[index].str || hashMap->data[index].status != BUSY || strcmp(str, hashMap->data[index].str)) {
        index = (index + hashMap->step) % hashMap->size;
        // if hashMap->data[index].status == WAS_NOT_USED there is no hope
        // to find the str further as all the following cells have WAS_NOT_USED
        // or filled with str of another hash in the linear chain
        if (hashMap->data[index].status == WAS_NOT_USED || index == indexStart)
            return hashMap->size;
    }
    return index;
}

size_t Hashify(const char* str, const size_t hashMapSize) {
    const int c = 21;
    size_t hash = 0;
    for (int i = 0; str[i]; i++)
        hash = c * hash + str[i];
    return hash % hashMapSize;
}

HashMap HashMapInit(const size_t size, const size_t step) {
    HashMap res;
    res.step = step;
    res.data = (Cell*)malloc(size * sizeof(Cell));
    res.size = res.data ? size : 0;
    for (size_t i = 0; i < res.size; i++) {
        res.data[i].str = NULL;
        res.data[i].status = WAS_NOT_USED;
    }
    return res;
}

bool HashMapInsert(HashMap* hashMap, const char* str) {
    size_t index = Hashify(str, hashMap->size);
    const size_t indexStart = index;
    long long firstDeleted = -1;
    while (hashMap->data[index].str) {
        if (!strcmp(hashMap->data[index].str, str) && hashMap->data[index].status == BUSY)
            return false;
        if (hashMap->data[index].status == FREE && firstDeleted == -1)
            firstDeleted = index;
        index = (index + hashMap->step) % hashMap->size;
        if (index == indexStart)
            return false;
    }
    if (firstDeleted != -1) {
        free(hashMap->data[firstDeleted].str);
        index = (size_t)firstDeleted;
    }
    hashMap->data[index].str = (char*)malloc((strlen(str) + 1) * sizeof(char));
    if (!hashMap->data[index].str)
        return false;
    strcpy(hashMap->data[index].str, str);
    hashMap->data[index].status = BUSY;
    return true;
}

bool HashMapFind(const HashMap* hashMap, const char* str) {
    return _FindIndexByStr(hashMap, str) != hashMap->size;
}

void HashMapRemove(HashMap* hashMap, const char* str) {
    size_t index = _FindIndexByStr(hashMap, str);
    if (index == hashMap->size)
        return;
    hashMap->data[index].status = FREE;
}

void HashMapDestroy(HashMap* hashMap) {
    for (size_t i = 0; i < hashMap->size; i++)
        if (hashMap->data[i].str)
            free(hashMap->data[i].str);
    free(hashMap->data);
    hashMap->data = NULL;
    hashMap->size = 0;
}

int TestSystemMainCycle(FILE* in, FILE* out) {
    HashMap map = HashMapInit(HASH_MAP_SIZE, HASH_MAP_STEP);
    if (map.size == 0)
        return -1;
    char buf[BUF_SIZE] = { 0 };
    char c = 0;
    char str[BUF_SIZE] = { 0 };
    while (fgets(buf, BUF_SIZE, in)) {
        sscanf(buf, "%c %s", &c, &str);
        switch (c) {
        case 'a':
            HashMapInsert(&map, str);
            break;
        case 'r':
            HashMapRemove(&map, str);
            break;
        case 'f':
            fprintf(out, "%s\n", HashMapFind(&map, str) ? "yes" : "no");
            break;
        default:
            HashMapDestroy(&map);
            return 0;
        }
    }
    HashMapDestroy(&map);
    return 0;
}
