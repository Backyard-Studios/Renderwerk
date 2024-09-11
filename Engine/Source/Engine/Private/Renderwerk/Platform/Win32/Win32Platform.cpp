#include "pch.h"

#include "Renderwerk/Platform/Win32/Win32Platform.h"

#include <Windows.h>

FWin32Platform::FWin32Platform() = default;

FWin32Platform::~FWin32Platform() = default;

void FWin32Platform::Initialize()
{
}

void FWin32Platform::Shutdown()
{
}

void* FWin32Platform::Allocate(const size_t Size, const uint8 Alignment)
{
	return _aligned_malloc(Size, Alignment);
}

void FWin32Platform::Free(void* Pointer, uint8 Alignment)
{
	_aligned_free(Pointer);
}

void FWin32Platform::Zero(void* Pointer, const size_t Size)
{
	ZeroMemory(Pointer, Size);
}

void FWin32Platform::Copy(void* Destination, const void* Source, const size_t Size)
{
	CopyMemory(Destination, Source, Size);
}

void FWin32Platform::Fill(void* Destination, const uint8 Value, const size_t Size)
{
	FillMemory(Destination, Size, Value);
}

void FWin32Platform::Move(void* Destination, const void* Source, const size_t Size)
{
	MoveMemory(Destination, Source, Size);
}

size_t FWin32Platform::GetAllocationSize(void* Pointer, const uint8 Alignment)
{
	return _aligned_msize(Pointer, Alignment, 0);
}

TSharedPtr<FWin32Platform> GetWin32Platform()
{
	return GetPlatform<FWin32Platform>();
}
