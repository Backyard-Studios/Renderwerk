#include "gtest/gtest.h"

#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Memory/UniquePointer.h"

TEST(UniquePointer, ValidMakeUniqueResult)
{
	uint8 Value = 5;
	TUniquePointer<uint8> UniquePointer = MakeUnique<uint8>(Value);
	EXPECT_NE(UniquePointer, nullptr);
	EXPECT_EQ(UniquePointer.IsValid(), true);
	EXPECT_EQ(*UniquePointer.GetRaw(), Value);
}

void UniquePointerTestFunction(TUniquePointer<uint8> UniquePointer, const uint8 Value)
{
	EXPECT_NE(UniquePointer, nullptr);
	EXPECT_EQ(UniquePointer.IsValid(), true);
	EXPECT_EQ(*UniquePointer.GetRaw(), Value);
}

TEST(UniquePointer, ScopeChangeMove)
{
	uint8 Value = 5;
	TUniquePointer<uint8> UniquePointer = MakeUnique<uint8>(Value);
	EXPECT_NE(UniquePointer, nullptr);
	EXPECT_EQ(UniquePointer.IsValid(), true);
	EXPECT_EQ(*UniquePointer.GetRaw(), Value);

	UniquePointerTestFunction(UniquePointer, Value);
	EXPECT_EQ(UniquePointer, nullptr);
	EXPECT_EQ(UniquePointer.IsValid(), false);
}

TEST(UniquePointer, ValidReset)
{
	uint8 Value = 5;
	TUniquePointer<uint8> UniquePointer = MakeUnique<uint8>(Value);
	EXPECT_NE(UniquePointer, nullptr);
	EXPECT_EQ(UniquePointer.IsValid(), true);
	EXPECT_EQ(*UniquePointer.GetRaw(), Value);

	UniquePointer.Reset();
	EXPECT_EQ(UniquePointer, nullptr);
	EXPECT_EQ(UniquePointer.IsValid(), false);
}
