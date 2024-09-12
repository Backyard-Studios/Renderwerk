#include "gtest/gtest.h"

#include "Renderwerk/Core/CoreDefinitions.h"

#if RW_ENABLE_MEMORY_TRACKING

#include "Renderwerk/Memory/Memory.h"

#include "Renderwerk/Platform/Platform.h"

TEST(Memory, UpdatesMemoryTrackingUsage)
{
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
