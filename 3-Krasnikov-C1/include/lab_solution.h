#ifndef __HEADER_H_
#define __HEADER_H_

#include <stdio.h>

#define MAX_INT_LENGTH 10
#define STACK_IS_EMPTY -1

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int* neighbours;
    int neighboursCount;
} NeighboursArray;

typedef struct {
    NeighboursArray* verticesNeighboursArrays;
    int verticesCount;
} AdjacencyList;

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

typedef struct {
    Node* top;
} Stack;

int IntLength(int num);
AdjacencyList* AdjacencyListReadFromStream(FILE* stream);
void AdjacencyListDestroy(AdjacencyList* adjacencyList);
bool DepthFirstTraversalIterative(FILE* stream, AdjacencyList* graph, int vertex);
int LabSolution();

Stack* StackInit();
Stack* StackPush(Stack* stack, int vertex);
int StackPop(Stack* stack);
bool StackIsEmpty(Stack* stack);
void StackDestroy(Stack* stack);
bool StackPushAllVertexNeighbours(Stack* stack, AdjacencyList* graph, int vertex);

#ifdef __cplusplus
}
#endif

#endif