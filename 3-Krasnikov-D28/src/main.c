#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define STACK_IS_EMPTY -1
#define WAGON_TO_IDX(wagon) ((wagon) - 1)
#define IDX_TO_WAGON(idx) ((idx) + 1)

typedef struct Node {
    int wagon;
    struct Node* next;
} Node;

int* g_wagonsTimes; // t(w)
int g_wagonsCount; // |W|
int g_timeBeforeDeparture; // D
int g_maximumWagonsLeft; // K
Node** g_wagonsRestrictions;

int g_curTime;
int g_minTime;
int g_maxWagonsUsed;
int* g_wagonsRestricted;
int* g_wagonsUsed;
int* g_curSolution;
int* g_bestSolution;

bool ListPush(Node** p_list, int wagon);
void ListPrint(Node* list);
void ListDestroy(Node* list);
void RestrictionsDestroy();

bool ReadInputData(const char* filename);

void CompressSearchSpace(int wagon);
void DecompressSearchSpace(int wagon);
void FindBestSolution(int wagon);
bool FindBestSolutionWrap();

int DistributeWagons(const char* filenameIn, const char* filenameOut);

int main(void) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    return DistributeWagons("input.txt", "output.txt");
}

bool ListPush(Node** p_list, int wagon) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode)
        return false;
    newNode->wagon = wagon;
    newNode->next = *p_list;
    *p_list = newNode;
    return true;
}

void ListPrint(Node* list) {
    Node* iter = list;
    while (iter) {
        printf("%d ", IDX_TO_WAGON(iter->wagon));
        iter = iter->next;
    }
}

void ListDestroy(Node* list) {
    Node* iter = list;
    while (iter) {
        Node* toFree = iter;
        iter = iter->next;
        free(toFree);
    }
}

void RestrictionsDestroy() {
    for (int i = 0; i < g_wagonsCount; i++)
        ListDestroy(g_wagonsRestrictions[i]);
    free(g_wagonsRestrictions);
}

bool ReadInputData(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file)
        return false;

    fscanf(file, "%d %d %d", &g_wagonsCount, &g_timeBeforeDeparture, &g_maximumWagonsLeft);

    g_wagonsTimes = (int*)malloc(g_wagonsCount * sizeof(int));
    if (!g_wagonsTimes) {
        fclose(file);
        return false;
    }

    g_wagonsRestrictions = (Node**)calloc(g_wagonsCount, sizeof(Node*));
    if (!g_wagonsRestrictions) {
        free(g_wagonsTimes);
        fclose(file);
        return false;
    }

    for (int i = 0; i < g_wagonsCount; i++)
        fscanf(file, "%d ", &g_wagonsTimes[i]);

    int pairsCount = 0;
    fscanf(file, "%d", &pairsCount);
    for (int i = 0; i < pairsCount; i++) {
        int before = 0, after = 0;
        fscanf(file, "%d %d", &before, &after);
        if (!ListPush(&g_wagonsRestrictions[WAGON_TO_IDX(after)], WAGON_TO_IDX(before))) {
            free(g_wagonsTimes);
            fclose(file);
            RestrictionsDestroy();
            return false;
        }
    }
    fclose(file);
    return true;
}

void CompressSearchSpace(int wagon) {
    g_wagonsUsed[wagon] = 1;
    Node* iter = g_wagonsRestrictions[wagon];
    while (iter) {
        g_wagonsRestricted[iter->wagon] = 1;
        iter = iter->next;
    }
}

void DecompressSearchSpace(int wagon) {
    g_wagonsUsed[wagon] = 0;
    Node* iter = g_wagonsRestrictions[wagon];
    while (iter) {
        g_wagonsRestricted[iter->wagon] = 0;
        iter = iter->next;
    }
}

void FindBestSolution(int m) {
    if (m >= g_wagonsCount - g_maximumWagonsLeft)
        if (m >= g_maxWagonsUsed && g_curTime < g_minTime) {
            g_maxWagonsUsed = m;
            g_minTime = g_curTime;
            memcpy(g_bestSolution, g_curSolution, g_wagonsCount * sizeof(int));
        }
    if (m < g_wagonsCount)
        for (int i = 0; i < g_wagonsCount; i++) {
            if (!g_wagonsRestricted[i] && !g_wagonsUsed[i]) {
                g_curSolution[m] = i;
                g_curTime += g_wagonsTimes[i];
                CompressSearchSpace(i);
                if (g_curTime <= g_timeBeforeDeparture)
                    FindBestSolution(m + 1);
                DecompressSearchSpace(i);
                g_curTime -= g_wagonsTimes[i];
            }
        }
}

bool FindBestSolutionWrap() {
    g_curTime = 0; 
    g_minTime = g_timeBeforeDeparture + 1; 
    g_maxWagonsUsed = 0;
    g_curSolution = (int*)malloc(g_wagonsCount * sizeof(int));
    if (!g_curSolution)
        return false;
    g_bestSolution = (int*)malloc(g_wagonsCount * sizeof(int));
    if (!g_bestSolution) {
        free(g_curSolution);
        return false;
    }
    g_wagonsRestricted = (int*)calloc(g_wagonsCount, sizeof(int));
    if (!g_wagonsRestricted) {
        free(g_curSolution);
        free(g_bestSolution);
        return false;
    }
    g_wagonsUsed = (int*)calloc(g_wagonsCount, sizeof(int));
    if (!g_wagonsRestricted) {
        free(g_curSolution);
        free(g_bestSolution);
        free(g_wagonsRestricted);
        return false;
    }
    FindBestSolution(0);
    free(g_curSolution);
    free(g_wagonsRestricted);
    free(g_wagonsUsed);
    return true;
}

int DistributeWagons(const char* filenameIn, const char* filenameOut) {
    FILE* fileOut = fopen(filenameOut, "w");
    if (!fileOut)
        return -1;

    ReadInputData(filenameIn); // allocating memory for times and restrictions

#ifdef DEBUG
    printf("g_wagonsTimes: ");
    for (int i = 0; i < g_wagonsCount; i++)
        printf("%d ", g_wagonsTimes[i]);
    printf("\n");
    printf("g_wagonsRestrictions:\n");
    for (int i = 0; i < g_wagonsCount; i++) {
        printf("Cannot be inserted after %d: ", IDX_TO_WAGON(i));
        ListPrint(g_wagonsRestrictions[i]);
        printf("\n");
    }
#endif

    if (FindBestSolutionWrap()) {
        if (g_maxWagonsUsed > 0) {
            for (int i = 0; i < g_maxWagonsUsed; i++)
                fprintf(fileOut, "%d ", IDX_TO_WAGON(g_bestSolution[i]));
            fprintf(fileOut, "\n");
        }
        else
            fprintf(fileOut, "0\n");
        free(g_bestSolution);
        free(g_wagonsTimes);
        RestrictionsDestroy();
        fclose(fileOut);
        return 0;
    }
    free(g_wagonsTimes);
    RestrictionsDestroy();
    fclose(fileOut);
    return -1;
}