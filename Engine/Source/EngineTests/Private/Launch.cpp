#include "gtest/gtest.h"

int main(int ArgumentCount, char** Arguments)
{
	testing::InitGoogleTest(&ArgumentCount, Arguments);
	return RUN_ALL_TESTS();
}
