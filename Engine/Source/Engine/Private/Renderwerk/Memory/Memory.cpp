#include "pch.h"

#include "Renderwerk/Memory/Memory.h"

#include <Windows.h>

void* FMemory::Allocate(const size64 Size, const size64 Alignment)
{
	return HeapAlloc(GetProcessHeap(), 0, CalculateAlignedSize(Size, Alignment));
}

void FMemory::Free(void* Memory)
{
	HeapFree(GetProcessHeap(), 0, Memory);
}

void* FMemory::Reallocate(void* Memory, const size64 Size, const size64 Alignment)
{
	return HeapReAlloc(GetProcessHeap(), 0, Memory, CalculateAlignedSize(Size, Alignment));
}

void FMemory::Copy(void* Destination, const void* Source, const size64 Size)
{
	CopyMemory(Destination, Source, Size);
}

size64 FMemory::GetSizeOfMemory(const void* Memory)
{
	return HeapSize(GetProcessHeap(), 0, Memory);
}

size64 FMemory::CalculateAlignedSize(const size64 Size, const size64 Alignment)
{
	return (Size + Alignment - 1) & ~(Alignment - 1);
}
