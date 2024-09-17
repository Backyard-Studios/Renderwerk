#include "gtest/gtest.h"

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Memory/SharedPointer.h"

TEST(SharedPointerTest, ValidMakeSharedResult)
{
	uint8 Value = 5;
	TSharedPointer<uint8> SharedPointer = MakeShared<uint8>(Value);
	EXPECT_NE(SharedPointer.GetRaw(), nullptr);
	EXPECT_EQ(SharedPointer.IsValid(), true);
	EXPECT_EQ(*SharedPointer.GetRaw(), Value);
	EXPECT_EQ(SharedPointer.GetReferenceCount(), 1);
}

void SharedPointerTestFunction(TSharedPointer<uint8> SharedPointer, const uint8 Value)
{
	EXPECT_NE(SharedPointer.GetRaw(), nullptr);
	EXPECT_EQ(SharedPointer.IsValid(), true);
	EXPECT_EQ(*SharedPointer.GetRaw(), Value);
	EXPECT_EQ(SharedPointer.GetReferenceCount(), 2);
}

TEST(SharedPointerTest, ScopeChangeMove)
{
	uint8 Value = 5;
	TSharedPointer<uint8> SharedPointer = MakeShared<uint8>(Value);
	EXPECT_NE(SharedPointer.GetRaw(), nullptr);
	EXPECT_EQ(SharedPointer.IsValid(), true);
	EXPECT_EQ(*SharedPointer.GetRaw(), Value);
	EXPECT_EQ(SharedPointer.GetReferenceCount(), 1);

	SharedPointerTestFunction(SharedPointer, Value);
	EXPECT_EQ(SharedPointer.GetReferenceCount(), 1);
}

TEST(SharedPointerTest, ValidReset)
{
	uint8 Value = 5;
	TSharedPointer<uint8> SharedPointer = MakeShared<uint8>(Value);
	EXPECT_NE(SharedPointer.GetRaw(), nullptr);
	EXPECT_EQ(SharedPointer.IsValid(), true);
	EXPECT_EQ(*SharedPointer.GetRaw(), Value);
	EXPECT_EQ(SharedPointer.GetReferenceCount(), 1);

	SharedPointer.Reset();
	EXPECT_EQ(SharedPointer.GetRaw(), nullptr);
	EXPECT_EQ(SharedPointer.IsValid(), false);
	EXPECT_EQ(SharedPointer.GetReferenceCount(), 0);
}
