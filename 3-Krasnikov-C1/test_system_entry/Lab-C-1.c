// FILE WHICH WAS DOWNLOADED ON TEST SYSTEM WEB SITE
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_INT_LENGTH 10
#define STACK_IS_EMPTY -1

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

typedef struct  {
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

int main() {
    return LabSolution();
}

int IntLength(int num) {
    int res = 0;
    while (num) {
        res++;
        num /= 10;
    }
    return res;
}

AdjacencyList* AdjacencyListReadFromStream(FILE* stream) {
    int verticesCount = 0, i = 0, currentVertex = 0, currentNeighbour = 0, * temp = NULL, lastNeighbourIdx = 0;
    int maxLineLength = 0;
    char* streamLineBuf = NULL, * streamLineBufIter = NULL;
    AdjacencyList* resAdjacencyList = NULL;
    char* verticesCountBuf = (char*)malloc(MAX_INT_LENGTH * sizeof(char));
    if (!verticesCountBuf)
        return NULL;
    // getting count of vertices
    fgets(verticesCountBuf, MAX_INT_LENGTH, stream);
    verticesCount = atoi(verticesCountBuf);
    free(verticesCountBuf);
    // allocating adjacency list
    resAdjacencyList = (AdjacencyList*)malloc(sizeof(AdjacencyList));
    if (!resAdjacencyList)
        return NULL;
    // inititalizing vertices count
    resAdjacencyList->verticesCount = verticesCount;
    resAdjacencyList->verticesNeighboursArrays = (NeighboursArray*)malloc(verticesCount * sizeof(NeighboursArray));
    if (!resAdjacencyList->verticesNeighboursArrays) {
        free(resAdjacencyList);
        return NULL;
    }
    // initializing all arrays with NULL
    for (i = 0; i < verticesCount; i++) {
        resAdjacencyList->verticesNeighboursArrays[i].neighbours = NULL;
        resAdjacencyList->verticesNeighboursArrays[i].neighboursCount = 0;
    }
    maxLineLength = (IntLength(verticesCount - 1) + 1) * verticesCount + 1;
    // initializing buffer with enough digits count
    streamLineBuf = (char*)malloc(maxLineLength * sizeof(char));
    if (!streamLineBuf) {
        AdjacencyListDestroy(resAdjacencyList);
        return NULL;
    }
    // reading input stream lines
    for (i = 0; i < verticesCount; i++) {
        fgets(streamLineBuf, maxLineLength, stream);
        streamLineBufIter = streamLineBuf;
        sscanf(streamLineBufIter, "%d", &currentVertex);
        // moving iter to the next number as sscanf doesn't do it
        while (isdigit(*streamLineBufIter))
            streamLineBufIter++;
        while (*streamLineBufIter == ' ')
            streamLineBufIter++;
        while (sscanf(streamLineBufIter, "%d", &currentNeighbour) > 0) {
            // moving iter to the next number as sscanf doesn't do it
            while (isdigit(*streamLineBufIter))
                streamLineBufIter++;
            while (*streamLineBufIter == ' ')
                streamLineBufIter++;
            // reallocating one more element in the appropriate array
            temp = (int*)realloc(resAdjacencyList->verticesNeighboursArrays[currentVertex].neighbours,
                ++resAdjacencyList->verticesNeighboursArrays[currentVertex].neighboursCount * sizeof(int));
            if (!temp) {
                free(streamLineBuf);
                AdjacencyListDestroy(resAdjacencyList);
                return NULL;
            }
            resAdjacencyList->verticesNeighboursArrays[currentVertex].neighbours = temp;
            lastNeighbourIdx = resAdjacencyList->verticesNeighboursArrays[currentVertex].neighboursCount - 1;
            resAdjacencyList->verticesNeighboursArrays[currentVertex].neighbours[lastNeighbourIdx] = currentNeighbour;
            // also adding currentVertex as a neighbour of currentNeighbour
            temp = (int*)realloc(resAdjacencyList->verticesNeighboursArrays[currentNeighbour].neighbours,
                ++resAdjacencyList->verticesNeighboursArrays[currentNeighbour].neighboursCount * sizeof(int));
            if (!temp) {
                free(streamLineBuf);
                AdjacencyListDestroy(resAdjacencyList);
                return NULL;
            }
            resAdjacencyList->verticesNeighboursArrays[currentNeighbour].neighbours = temp;
            lastNeighbourIdx = resAdjacencyList->verticesNeighboursArrays[currentNeighbour].neighboursCount - 1;
            resAdjacencyList->verticesNeighboursArrays[currentNeighbour].neighbours[lastNeighbourIdx] = currentVertex;
        }
    }
    free(streamLineBuf);
    return resAdjacencyList;
}

void AdjacencyListDestroy(AdjacencyList* adjacencyList) {
    int i = 0;
    for (i = 0; i < adjacencyList->verticesCount; i++)
        if (adjacencyList->verticesNeighboursArrays[i].neighbours)
            free(adjacencyList->verticesNeighboursArrays[i].neighbours);
    free(adjacencyList->verticesNeighboursArrays);
    free(adjacencyList);
}

bool DepthFirstTraversalIterative(FILE* stream, AdjacencyList* graph, int vertex) {
    int* visited = NULL;
    int currentVertex = 0;
    Stack* stack = StackInit();
    if (!stack)
        return false;
    visited = (int*)calloc(graph->verticesCount, sizeof(int));
    if (!visited) {
        StackDestroy(stack);
        return false;
    }
    visited[vertex] = 1;
    fprintf(stream, "%d ", vertex);
    // pushing all vertex neighbours in pre-order
    if (!StackPushAllVertexNeighbours(stack, graph, vertex)) {
        StackDestroy(stack);
        return false;
    }
    while (!StackIsEmpty(stack)) {
        currentVertex = StackPop(stack);
        if (!visited[currentVertex]) {
            visited[currentVertex] = 1;
            fprintf(stream, "%d ", currentVertex);
            if (!StackPushAllVertexNeighbours(stack, graph, currentVertex)) {
                StackDestroy(stack);
                return false;
            }
        }
    }
    free(visited);
    StackDestroy(stack);
    return true;
}

int LabSolution() {
    AdjacencyList* graph = AdjacencyListReadFromStream(stdin);
    if (!graph)
        return -1;
    if (!DepthFirstTraversalIterative(stdout, graph, 0)) {
        AdjacencyListDestroy(graph);
        return -1;
    }
    AdjacencyListDestroy(graph);
    return 0;
}

Stack* StackInit() {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (!stack)
        return NULL;
    stack->top = NULL;
    return stack;
}

Stack* StackPush(Stack* stack, int vertex) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode)
        return NULL;
    newNode->vertex = vertex;
    newNode->next = stack->top;
    stack->top = newNode;
    return stack;
}

int StackPop(Stack* stack) {
    if (!StackIsEmpty(stack)) {
        Node* nodeToFree = stack->top;
        int vertex = nodeToFree->vertex;
        stack->top = stack->top->next;
        free(nodeToFree);
        return vertex;
    }
    return STACK_IS_EMPTY;
}

bool StackIsEmpty(Stack* stack) {
    return !stack->top;
}

void StackDestroy(Stack* stack) {
    Node* iter = stack->top;
    while (iter) {
        Node* nodeToFree = iter;
        iter = iter->next;
        free(iter);
    }
    free(stack);
}

bool StackPushAllVertexNeighbours(Stack* stack, AdjacencyList* graph, int vertex) {
    int i = graph->verticesNeighboursArrays[vertex].neighboursCount - 1;
    for (; i >= 0; i--) {
        int neighbour = graph->verticesNeighboursArrays[vertex].neighbours[i];
        if (!StackPush(stack, neighbour)) {
            return false;
        }
    }
    return true;
}