#define _CRT_SECURE_NO_WARNINGS

#include <gtest/gtest.h>
#include <lab_solution.h>

/*
IDE: Visual Studio 2019 Community Edition
OS: Windows 10 Pro Ver. 20H2 Build 19042.1288
PC configuration:
    CPU:            AMD Ryzen 7 3700X 8-Core Processor 3.59 GHz
    Motherboard:    GIGABYTE B450 AORUS ELITE
    RAM:            Kingston HyperX Fury 2x8 Gb (16 Gb) dual-channel (Intel XPM profile 3200 MHz on)
    HDD:            WD Blue 1TB - this is the hard drive on which test files are being read/written
*/

/*
LabSolution_StressTest results (run on Release VS configuration):
    Time:   4.718 sec to generate graph
            20.281 sec to output answer
    RAM:    about 1.3 GB at peak
*/

const char* filenameGraph = "StressTestGraph.txt";
const int verticesCount = (int)1e6;

TEST(LabSolution_StressTest, LabSolutionGraphGenerating) {
    const int maxDeltaNeighbour = verticesCount;
    // opening test file
    FILE* fileGraph = fopen(filenameGraph, "w");
    if (!fileGraph) {
        printf("Error while opening %s file!\n", filenameGraph);
        ASSERT_TRUE(fileGraph);
    }
    // generating random graph 
    fprintf(fileGraph, "%d\n", verticesCount);
    for (int i = 0; i < verticesCount - 1; i++) {
        fprintf(fileGraph, "%d ", i);
        for (int neighbour = i + rand() % (maxDeltaNeighbour - 1) + 1; neighbour < verticesCount; neighbour += rand() % (maxDeltaNeighbour - 1) + 1)
            fprintf(fileGraph, "%d ", neighbour);
        fprintf(fileGraph, "\n");
    }
    fprintf(fileGraph, "%d\n", verticesCount - 1);
    fclose(fileGraph);
}

TEST(LabSolution_StressTest, LabSolutionMainStressTest) {
    const char* filenameOutput = "Output.txt";
    bool depthFirstTraversalResult = false;
    FILE* fileGraph = fopen(filenameGraph, "r");
    if (!fileGraph) {
        printf("Error while opening %s file!\n", filenameGraph);
        ASSERT_TRUE(fileGraph);
    }
    AdjacencyList* graph = AdjacencyListReadFromStream(fileGraph);
    fclose(fileGraph);
    if (!graph) {
        printf("Failed to read graph!\n");
        ASSERT_TRUE(graph);
    }
    FILE* fileOutput = fopen(filenameOutput, "w");
    if (!fileOutput) {
        AdjacencyListDestroy(graph);
        printf("Error while opening %s file!\n", filenameOutput);
        ASSERT_TRUE(fileOutput);
    }
    depthFirstTraversalResult = DepthFirstTraversalIterative(fileOutput, graph, 0);
    if (!depthFirstTraversalResult) {
        AdjacencyListDestroy(graph);
        printf("Memory allocation failed while DepthFirstTraversalIterative!");
        ASSERT_TRUE(depthFirstTraversalResult);
    }
    fclose(fileOutput);
    AdjacencyListDestroy(graph);
}