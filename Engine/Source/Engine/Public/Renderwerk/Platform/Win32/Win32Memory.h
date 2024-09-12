#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"

class ENGINE_API FWin32Memory
{
public:
	static void* Allocate(size64 Size, uint8 Alignment);
	static void Free(void* Pointer, uint8 Alignment);
	static void Zero(void* Pointer, size64 Size);
	static void Copy(void* Destination, const void* Source, size64 Size);
	static void Fill(void* Destination, uint8 Value, size64 Size);
	static void Move(void* Destination, const void* Source, size64 Size);
	static size64 GetAllocationSize(void* Pointer, uint8 Alignment);
};
