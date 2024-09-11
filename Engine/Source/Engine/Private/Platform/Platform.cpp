#include "pch.h"

#include "Platform/Platform.h"

#include <cassert>

IPlatform* GPlatform = nullptr;

IPlatform* GetPlatform()
{
	assert(GPlatform != nullptr);
	return GPlatform;
}
