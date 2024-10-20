#include "pch.h"

#include "Renderwerk/Memory/Memory.h"

#include <Windows.h>

#if RW_ENABLE_MEMORY_TRACKING
FMemoryStatistics* FMemory::MemoryStatistics = nullptr;
#endif

void* FMemory::Allocate(const size64 Size, const size64 Alignment)
{
	DEBUG_ASSERTM(Size > 0, "Size must be greater than 0");
	DEBUG_ASSERTM(Alignment % 2 == 0, "Alignment must be a power of 2");
	size64 AlignedSize = CalculateAlignedSize(Size, Alignment);
	void* Pointer = HeapAlloc(GetProcessHeap(), 0, CalculateAlignedSize(Size, Alignment));
#if RW_ENABLE_MEMORY_TRACKING
	if (!MemoryStatistics)
		MemoryStatistics = new FMemoryStatistics();
	MemoryStatistics->CurrentUsage += AlignedSize;
	if (RW_IS_PROFILER_STARTED())
		RW_PROFILING_MEMORY_ALLOCATION(Pointer, AlignedSize);
#endif
	return Pointer;
}

void FMemory::Free(void* Memory)
{
	DEBUG_ASSERTM(Memory, "Memory must not be null");
#if RW_ENABLE_MEMORY_TRACKING
	if (!MemoryStatistics)
		MemoryStatistics = new FMemoryStatistics();
	MemoryStatistics->CurrentUsage -= GetSizeOfMemory(Memory);
	if (RW_IS_PROFILER_STARTED())
		RW_PROFILING_MEMORY_FREE(Memory);
#endif
	HeapFree(GetProcessHeap(), 0, Memory);
}

void* FMemory::Reallocate(void* Memory, const size64 Size, const size64 Alignment)
{
	DEBUG_ASSERTM(Memory, "Memory must not be null");
	DEBUG_ASSERTM(Size > 0, "Size must be greater than 0");
	DEBUG_ASSERTM(Alignment % 2 == 0, "Alignment must be a power of 2");
	return HeapReAlloc(GetProcessHeap(), 0, Memory, CalculateAlignedSize(Size, Alignment));
}

void FMemory::Copy(void* Destination, const void* Source, const size64 Size)
{
	DEBUG_ASSERTM(Destination, "Destination must not be null");
	DEBUG_ASSERTM(Source, "Source must not be null");
	DEBUG_ASSERTM(Size > 0, "Size must be greater than 0");
	CopyMemory(Destination, Source, Size);
}

size64 FMemory::GetSizeOfMemory(const void* Memory)
{
	DEBUG_ASSERTM(Memory, "Memory must not be null");
	return HeapSize(GetProcessHeap(), 0, Memory);
}

size64 FMemory::CalculateAlignedSize(const size64 Size, const size64 Alignment)
{
	DEBUG_ASSERTM(Size > 0, "Size must be greater than 0");
	DEBUG_ASSERTM(Alignment % 2 == 0, "Alignment must be a power of 2");
	return (Size + Alignment - 1) & ~(Alignment - 1);
}

#if RW_ENABLE_MEMORY_TRACKING
FMemoryStatistics* FMemory::GetMemoryStatistics()
{
	DEBUG_ASSERTM(MemoryStatistics, "MemoryStatistics must not be null");
	return MemoryStatistics;
}
#endif
