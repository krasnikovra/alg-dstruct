#define _CRT_SECURE_NO_WARNINGS

#include <gtest/gtest.h>
#include "..\include\lab_solution.h"

#define MAX_WAGONS_COUNT_IN_FUNC_TEST 10
#define SOME_GARBAGE_BYTE 0xAF

int ReadOutputData(const char* filenameOut, int* solution, int size) {
	FILE* fileOut = fopen(filenameOut, "r");
	if (!fileOut)
		return 0;
	for (int i = 0; i < size; i++) 
		if (!fscanf(fileOut, "%d", &solution[i]))
			break;
	fclose(fileOut);
	return 1;
}

void TestSuite(const char* filenameIn, const char* filenameOut, const char* filenameCorrectOut) {
	int distWagError = DistributeWagons(filenameIn, filenameOut);
	if (!distWagError) {
		int solution[MAX_WAGONS_COUNT_IN_FUNC_TEST] = { 0 }, correct_solution[MAX_WAGONS_COUNT_IN_FUNC_TEST] = { 0 };
		memset(solution, SOME_GARBAGE_BYTE, MAX_WAGONS_COUNT_IN_FUNC_TEST * sizeof(int));
		memset(correct_solution, SOME_GARBAGE_BYTE, MAX_WAGONS_COUNT_IN_FUNC_TEST * sizeof(int));
		ASSERT_TRUE(ReadOutputData(filenameOut, solution, MAX_WAGONS_COUNT_IN_FUNC_TEST));
		ASSERT_TRUE(ReadOutputData(filenameCorrectOut, correct_solution, MAX_WAGONS_COUNT_IN_FUNC_TEST));
		EXPECT_TRUE(!memcmp(solution, correct_solution, MAX_WAGONS_COUNT_IN_FUNC_TEST * sizeof(int)));
	}
	else
		ASSERT_TRUE(!distWagError);
}

class FunctionalTest : public ::testing::Test {
protected:
	void SetUp() {
		_CrtMemCheckpoint(&m_before);
	}
	void TearDown() {
		_CrtMemCheckpoint(&m_after);
		if (_CrtMemDifference(&m_diff, &m_before, &m_after)) {
			_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
			_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
			_CrtMemDumpStatistics(&m_diff);
			FAIL();
		}
	}
private:
	_CrtMemState m_before, m_after, m_diff;
};

TEST_F(FunctionalTest, PdfExampleTest) {
// Example from lab d28 task text
	TestSuite(
		"test_data/PdfExampleTest/input.txt",
		"test_data/PdfExampleTest/output.txt",
		"test_data/PdfExampleTest/correct_output.txt"
	);
}

TEST_F(FunctionalTest, NotEnoughTimeTest) {
// K = 1 so we need to attach at least 3 wagons but all combinations of 3 wagons
// have total time more than given
	TestSuite(
		"test_data/NotEnoughTimeTest/input.txt",
		"test_data/NotEnoughTimeTest/output.txt",
		"test_data/NotEnoughTimeTest/correct_output.txt"
	);
}

TEST_F(FunctionalTest, WagonsOrderConflictTest) {
// Here we have conflict in wagons order condition so we cant attach all 4 wagons,
// but K = 0 is given so there is no solution
	TestSuite(
		"test_data/WagonsOrderConflictTest/input.txt",
		"test_data/WagonsOrderConflictTest/output.txt",
		"test_data/WagonsOrderConflictTest/correct_output.txt"
	);
}

TEST_F(FunctionalTest, MaximumWagonsTest) {
// Here we have several solutions (1, 2) (1, 3) (2, 3) but only one (1, 2, 3) with maximum wagons which is the right answer
	TestSuite(
		"test_data/MaximumWagonsTest/input.txt",
		"test_data/MaximumWagonsTest/output.txt",
		"test_data/MaximumWagonsTest/correct_output.txt"
	);
}

TEST_F(FunctionalTest, MinimumTimeTest) {
// Here we have three solutions (1, 2) (1, 3) (2, 3) with times 8, 7, 9 and second has minimum time
	TestSuite(
		"test_data/MinimumTimeTest/input.txt",
		"test_data/MinimumTimeTest/output.txt",
		"test_data/MinimumTimeTest/correct_output.txt"
	);
}
