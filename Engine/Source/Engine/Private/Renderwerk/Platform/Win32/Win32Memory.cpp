#include "pch.h"

#include "Renderwerk/Platform/Win32/Win32Memory.h"

void* FWin32Memory::Allocate(const size64 Size, const uint8 Alignment)
{
	return _aligned_malloc(Size, Alignment);
}

void FWin32Memory::Free(void* Pointer, uint8 Alignment)
{
	_aligned_free(Pointer);
}

void FWin32Memory::Zero(void* Pointer, const size64 Size)
{
	ZeroMemory(Pointer, Size);
}

void FWin32Memory::Copy(void* Destination, const void* Source, const size64 Size)
{
	CopyMemory(Destination, Source, Size);
}

void FWin32Memory::Fill(void* Destination, const uint8 Value, const size64 Size)
{
	FillMemory(Destination, Size, Value);
}

void FWin32Memory::Move(void* Destination, const void* Source, const size64 Size)
{
	MoveMemory(Destination, Source, Size);
}

size64 FWin32Memory::GetAllocationSize(void* Pointer, const uint8 Alignment)
{
	return _aligned_msize(Pointer, Alignment, 0);
}
