#define _CRT_SECURE_NO_WARNINGS

#include <gtest/gtest.h>
#include "..\include\lab_solution.h"

int ReadOutputData(const char* filenameOut, int* solution, int size) {
	FILE* fileOut = fopen(filenameOut, "r");
	if (!fileOut)
		return 0;
	for (int i = 0; i < size; i++) {
		if (!fscanf(fileOut, "%d", &solution[i]))
			break;
	}
	fclose(fileOut);
	return 1;
}

#define MAX_WAGONS_COUNT_IN_FUNC_TEST 10

void TestSuite(const char* filenameIn, const char* filenameOut, const char* filenameCorrectOut) {
	int distWagError = DistributeWagons(
		filenameIn,
		filenameOut
	);
	if (!distWagError) {
		int solution[MAX_WAGONS_COUNT_IN_FUNC_TEST], correct_solution[MAX_WAGONS_COUNT_IN_FUNC_TEST];
		ASSERT_TRUE(ReadOutputData(filenameOut, solution, MAX_WAGONS_COUNT_IN_FUNC_TEST));
		ASSERT_TRUE(ReadOutputData(filenameCorrectOut, correct_solution, MAX_WAGONS_COUNT_IN_FUNC_TEST));
		EXPECT_TRUE(!memcmp(solution, correct_solution, MAX_WAGONS_COUNT_IN_FUNC_TEST * sizeof(int)));
	}
	else {
		ASSERT_TRUE(distWagError);
	}
}

TEST(FunctionalTest, PdfExampleTest) {
// Example from lab d28 task text
	TestSuite(
		"test_data/PdfExampleTest/input.txt",
		"test_data/PdfExampleTest/output.txt",
		"test_data/PdfExampleTest/correct_output.txt"
	);
}

TEST(FunctionalTest, NotEnoughTimeTest) {
// K = 1 so we need to attach at least 3 wagons but all combinations of 3 wagons
// have total time more than given
	TestSuite(
		"test_data/NotEnoughTimeTest/input.txt",
		"test_data/NotEnoughTimeTest/output.txt",
		"test_data/NotEnoughTimeTest/correct_output.txt"
	);
}

TEST(FunctionalTest, WagonsOrderConflictTest) {
// Here we have confliction wagons order condition so we cant attach all 4 wagons,
// but K = 0 is given so there is no solution
	TestSuite(
		"test_data/WagonsOrderConflictTest/input.txt",
		"test_data/WagonsOrderConflictTest/output.txt",
		"test_data/WagonsOrderConflictTest/correct_output.txt"
	);
}
