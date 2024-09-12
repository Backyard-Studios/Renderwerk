#include "gtest/gtest.h"

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Memory/UniquePointer.h"

TEST(UniquePointerTest, ValidMakeUniqueResult)
{
	uint8 Value = 5;
	TUniquePointer<uint8> UniquePointer = MakeUnique<uint8>(Value);
	EXPECT_NE(UniquePointer.GetRaw(), nullptr);
	EXPECT_EQ(UniquePointer.IsValid(), true);
	EXPECT_EQ(*UniquePointer.GetRaw(), Value);
}

void UniquePointerTestFunction(TUniquePointer<uint8> UniquePointer, const uint8 Value)
{
	EXPECT_NE(UniquePointer.GetRaw(), nullptr);
	EXPECT_EQ(UniquePointer.IsValid(), true);
	EXPECT_EQ(*UniquePointer.GetRaw(), Value);
}

TEST(UniquePointerTest, ScopeChangeMove)
{
	uint8 Value = 5;
	TUniquePointer<uint8> UniquePointer = MakeUnique<uint8>(Value);
	EXPECT_NE(UniquePointer.GetRaw(), nullptr);
	EXPECT_EQ(UniquePointer.IsValid(), true);
	EXPECT_EQ(*UniquePointer.GetRaw(), Value);

	UniquePointerTestFunction(std::move(UniquePointer), Value);
}

TEST(UniquePointerTest, ValidReset)
{
	uint8 Value = 5;
	TUniquePointer<uint8> UniquePointer = MakeUnique<uint8>(Value);
	EXPECT_NE(UniquePointer.GetRaw(), nullptr);
	EXPECT_EQ(UniquePointer.IsValid(), true);
	EXPECT_EQ(*UniquePointer.GetRaw(), Value);

	UniquePointer.Reset();
	EXPECT_EQ(UniquePointer.GetRaw(), nullptr);
	EXPECT_EQ(UniquePointer.IsValid(), false);
}
