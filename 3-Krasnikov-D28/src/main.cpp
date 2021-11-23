#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>
#include <gtest/gtest.h>
#include "..\include\lab_solution.h"

#define _TESTS

int main(int argc, char** argv) {
#ifdef _TESTS
	::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
	return 0;
#else
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	return DistributeWagons("input.txt", "output.txt");
#endif
}