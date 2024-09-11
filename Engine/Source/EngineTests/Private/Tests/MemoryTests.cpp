#include "gtest/gtest.h"

#include "Renderwerk/Core/CoreDefinitions.h"

#if RW_ENABLE_MEMORY_TRACKING

#include "Renderwerk/Memory/Memory.h"

#include "Renderwerk/Platform/Platform.h"

#if RW_PLATFORM_WINDOWS
#	include "Renderwerk/Platform/Win32/Win32Platform.h"
#endif

TEST(Memory, UpdatesMemoryTrackingUsage)
{
#if RW_PLATFORM_WINDOWS
	GPlatform = MakeShared<FWin32Platform>();
#endif

	FMemoryTracking& MemoryTracking = FMemory::GetMemoryTracking();
	EXPECT_EQ(MemoryTracking.GetUsage(), 0);

	size64 AllocationSize = 10;
	void* Memory = FMemory::Allocate(AllocationSize);
	EXPECT_EQ(MemoryTracking.GetUsage(), AllocationSize);

	FMemory::Free(Memory);
	EXPECT_EQ(MemoryTracking.GetUsage(), 0);
}

TEST(Memory, CallsMemoryTrackingCallbacks)
{
#if RW_PLATFORM_WINDOWS
	GPlatform = MakeShared<FWin32Platform>();
#endif

	bool bWasCallbackCalled = false;
	FMemoryTracking& MemoryTracking = FMemory::GetMemoryTracking();
	MemoryTracking.SetOnAllocateCallback([&bWasCallbackCalled](void* Pointer, size64 Size, uint8 Alignment) { bWasCallbackCalled = true; });
	MemoryTracking.SetOnFreeCallback([&bWasCallbackCalled](void* Pointer, uint8 Alignment) { bWasCallbackCalled = true; });
	EXPECT_EQ(bWasCallbackCalled, false);

	void* Memory = FMemory::Allocate(10);
	EXPECT_EQ(bWasCallbackCalled, true);

	bWasCallbackCalled = false;
	FMemory::Free(Memory);
	EXPECT_EQ(bWasCallbackCalled, true);
}

#endif
