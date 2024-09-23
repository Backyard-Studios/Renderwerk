#include "pch.h"

#include "Renderwerk/Memory/Memory.h"

#if RW_ENABLE_MEMORY_TRACKING

void FMemoryTracking::SetOnAllocateCallback(const FOnAllocateCallback& Callback)
{
	OnAllocateCallback = Callback;
}

void FMemoryTracking::SetOnFreeCallback(const FOnFreeCallback& Callback)
{
	OnFreeCallback = Callback;
}

void FMemoryTracking::OnAllocate(void* Pointer, const size64 Size, const uint8 Alignment)
{
	Usage += Size;
	PeakUsage = std::max(Usage, PeakUsage);
	if (OnAllocateCallback)
		OnAllocateCallback(Pointer, Size, Alignment);
}

void FMemoryTracking::OnFree(void* Pointer, const uint8 Alignment)
{
	Usage -= _aligned_msize(Pointer, Alignment, 0);
	if (OnFreeCallback)
		OnFreeCallback(Pointer, Alignment);
}

#endif
