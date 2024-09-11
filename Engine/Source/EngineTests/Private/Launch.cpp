#include "Test.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

TEST(Engine, TestFunction)
{
	int Value = 42;
	int Expected = Value * 2;
	
	int Result = TestFunction(Value);
	
	EXPECT_EQ(Result, Expected);
}

int main(int ArgumentCount, char **Arguments) {
	testing::InitGoogleTest(&ArgumentCount, Arguments);
	return RUN_ALL_TESTS();
}
