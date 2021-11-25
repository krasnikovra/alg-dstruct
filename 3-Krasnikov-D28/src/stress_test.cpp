#define _CRT_SECURE_NO_WARNINGS

#include <gtest/gtest.h>
#include "..\include\lab_solution.h"

#define WAGONS_COUNT 13
#define TIME_BEFORE_DEPARTURE 120
#define MAX_WAGONS_LEFT 3
#define ORDERS_COND_DIVIDER 4
#define STRESS_TEST_INPUT "test_data/StressTest/input.txt"
#define STRESS_TEST_OUTPUT "test_data/StressTest/output.txt"

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
StressTest results (run on Release VS configuration):
    Time:   44.249 sec to output answer
    RAM:    1 Mb reserved for system stack
            772 Kb according to VS memory metrics
*/

TEST(StressTest, GenerateInputTest) {
    FILE* fileInput = fopen(STRESS_TEST_INPUT, "w");
    ASSERT_TRUE(fileInput);
    fprintf(fileInput, "%d %d %d\n", WAGONS_COUNT, TIME_BEFORE_DEPARTURE, MAX_WAGONS_LEFT);
    for (int i = 0; i < WAGONS_COUNT; i++)
        fprintf(fileInput, "%d ", (rand() % (TIME_BEFORE_DEPARTURE / WAGONS_COUNT - 1) + 1));
    fprintf(fileInput, "\n");
    int orderCondsCount = WAGONS_COUNT / ORDERS_COND_DIVIDER;
    int beforeDelta = orderCondsCount > 0 ? WAGONS_COUNT / orderCondsCount - 1 : 0;
    int before = 1;
    fprintf(fileInput, "%d\n", orderCondsCount);
    for (int i = 0; i < orderCondsCount; i++) {
        before += beforeDelta;
        int after = rand() % (WAGONS_COUNT - 1) + 1;
        fprintf(fileInput, "%d %d\n", before, after != before ? after : (after > 1 ? after - 1 : after + 1));
    }
    fclose(fileInput);
}

TEST(StressTest, SolutionStressTest) {
    ASSERT_TRUE(!DistributeWagons(STRESS_TEST_INPUT, STRESS_TEST_OUTPUT));
}