#include <gtest/gtest.h>
#include "lab_solution.h"

#define STRESS_TEST

int main(int argc, char** argv) {
#ifdef STRESS_TEST
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
#else
    return LabSolution();
#endif
}