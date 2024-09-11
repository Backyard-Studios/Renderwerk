#include "pch.h"

#include "Renderwerk/Platform/Platform.h"

#include <cassert>

IPlatform* GPlatform = nullptr;

IPlatform* GetPlatform()
{
	assert(GPlatform != nullptr);
	return GPlatform;
}
