#define _CRT_SECURE_NO_WARNINGS

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "..\include\lab_solution.h"

#define WAGON_TO_IDX(wagon) ((wagon) - 1)
#define IDX_TO_WAGON(idx) ((idx) + 1)

typedef struct Node {
    unsigned wagon;
    struct Node* next;
} Node;

unsigned* g_wagonsTimes; // t(w)
unsigned g_wagonsCount; // |W|
unsigned g_timeBeforeDeparture; // D
unsigned g_maximumWagonsLeft; // K
Node** g_wagonsRestrictions;

unsigned g_curTime;
unsigned g_minTime;
unsigned g_maxWagonsUsed;
unsigned* g_wagonsRestricted;
unsigned* g_wagonsUsed;
unsigned* g_curSolution;
unsigned* g_bestSolution;

bool ListPush(Node** p_list, unsigned wagon) {
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
    for (unsigned i = 0; i < g_wagonsCount; i++)
        ListDestroy(g_wagonsRestrictions[i]);
    free(g_wagonsRestrictions);
}

bool ReadInputData(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file)
        return false;

    fscanf(file, "%d %d %d", &g_wagonsCount, &g_timeBeforeDeparture, &g_maximumWagonsLeft);

    g_wagonsTimes = (unsigned*)malloc(g_wagonsCount * sizeof(unsigned));
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

    for (unsigned i = 0; i < g_wagonsCount; i++)
        fscanf(file, "%d ", &g_wagonsTimes[i]);

    unsigned pairsCount = 0;
    fscanf(file, "%d", &pairsCount);
    for (unsigned i = 0; i < pairsCount; i++) {
        unsigned before = 0, after = 0;
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

void CompressSearchSpace(unsigned wagon) {
    g_wagonsUsed[wagon] = 1;
    Node* iter = g_wagonsRestrictions[wagon];
    while (iter) {
        g_wagonsRestricted[iter->wagon] = 1;
        iter = iter->next;
    }
}

void DecompressSearchSpace(unsigned wagon) {
    g_wagonsUsed[wagon] = 0;
    Node* iter = g_wagonsRestrictions[wagon];
    while (iter) {
        g_wagonsRestricted[iter->wagon] = 0;
        iter = iter->next;
    }
}

void FindBestSolution(unsigned m) {
    if (m >= g_wagonsCount - g_maximumWagonsLeft)
        if (m == g_maxWagonsUsed && g_curTime < g_minTime) {
            g_minTime = g_curTime;
            memcpy(g_bestSolution, g_curSolution, g_wagonsCount * sizeof(unsigned));
        }
        else if (m > g_maxWagonsUsed) {
            g_maxWagonsUsed = m;
            g_minTime = g_curTime;
            memcpy(g_bestSolution, g_curSolution, g_wagonsCount * sizeof(unsigned));
        }
    if (m < g_wagonsCount)
        for (unsigned i = 0; i < g_wagonsCount; i++) {
            if (!g_wagonsRestricted[i] && !g_wagonsUsed[i] && g_curTime + g_wagonsTimes[i] <= g_timeBeforeDeparture) {
                g_curSolution[m] = i;
                g_curTime += g_wagonsTimes[i];
                CompressSearchSpace(i);
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
    g_curSolution = (unsigned*)malloc(g_wagonsCount * sizeof(unsigned));
    if (!g_curSolution)
        return false;
    g_bestSolution = (unsigned*)malloc(g_wagonsCount * sizeof(unsigned));
    if (!g_bestSolution) {
        free(g_curSolution);
        return false;
    }
    g_wagonsRestricted = (unsigned*)calloc(g_wagonsCount, sizeof(unsigned));
    if (!g_wagonsRestricted) {
        free(g_curSolution);
        free(g_bestSolution);
        return false;
    }
    g_wagonsUsed = (unsigned*)calloc(g_wagonsCount, sizeof(unsigned));
    if (!g_wagonsUsed) {
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

unsigned DistributeWagons(const char* filenameIn, const char* filenameOut) {
    if (!ReadInputData(filenameIn)) // allocating memory for times and restrictions
        return 1;

#ifdef DEBUG
    printf("g_wagonsTimes: ");
    for (unsigned i = 0; i < g_wagonsCount; i++)
        printf("%d ", g_wagonsTimes[i]);
    printf("\n");
    printf("g_wagonsRestrictions:\n");
    for (unsigned i = 0; i < g_wagonsCount; i++) {
        printf("Cannot be inserted after %d: ", IDX_TO_WAGON(i));
        ListPrint(g_wagonsRestrictions[i]);
        printf("\n");
    }
#endif

    FILE* fileOut = fopen(filenameOut, "w");
    if (!fileOut)
        return 1;

    if (FindBestSolutionWrap()) {
        if (g_maxWagonsUsed > 0) {
            for (unsigned i = 0; i < g_maxWagonsUsed; i++)
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
    return 1;
}