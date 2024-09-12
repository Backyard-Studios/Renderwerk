#include "gtest/gtest.h"

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Memory/UniquePointer.h"

#if RW_PLATFORM_WINDOWS
#	include "Renderwerk/Platform/Win32/Win32Platform.h"
#endif

class FUniquePointerTest : public testing::Test
{
protected:
	FUniquePointerTest() = default;
	~FUniquePointerTest() override = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FUniquePointerTest)

public:
	void SetUp() override
	{
#if RW_PLATFORM_WINDOWS
		if (GPlatform == nullptr)
			GPlatform = MakeShared<FWin32Platform>();
#endif
	}

	void TearDown() override
	{
	}
};

TEST_F(FUniquePointerTest, ValidMakeUniqueResult)
{
	uint8 Value = 5;
	TUniquePointer<uint8> UniquePointer = MakeUnique<uint8>(Value);
	EXPECT_NE(UniquePointer.GetRaw(), nullptr);
	EXPECT_EQ(UniquePointer.IsValid(), true);
	EXPECT_EQ(*UniquePointer.GetRaw(), Value);

	// We have to reset the unique pointer here because the platform is reset in TearDown.
	UniquePointer.Reset();
}

void UniquePointerTestFunction(TUniquePointer<uint8> UniquePointer, const uint8 Value)
{
	EXPECT_NE(UniquePointer.GetRaw(), nullptr);
	EXPECT_EQ(UniquePointer.IsValid(), true);
	EXPECT_EQ(*UniquePointer.GetRaw(), Value);

	// We have to reset the unique pointer here because the platform is reset in TearDown.
	UniquePointer.Reset();
}

TEST_F(FUniquePointerTest, ScopeChangeMove)
{
	uint8 Value = 5;
	TUniquePointer<uint8> UniquePointer = MakeUnique<uint8>(Value);
	EXPECT_NE(UniquePointer.GetRaw(), nullptr);
	EXPECT_EQ(UniquePointer.IsValid(), true);
	EXPECT_EQ(*UniquePointer.GetRaw(), Value);

	UniquePointerTestFunction(std::move(UniquePointer), Value);
}

TEST_F(FUniquePointerTest, ValidReset)
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
