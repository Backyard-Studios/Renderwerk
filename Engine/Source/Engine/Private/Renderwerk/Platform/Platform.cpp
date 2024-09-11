#include "pch.h"

#include "Renderwerk/Platform/Platform.h"

#include <cassert>

TSharedPtr<IPlatform> GPlatform = nullptr;

TSharedPtr<IPlatform> GetPlatform()
{
	assert(GPlatform != nullptr);
	return GPlatform;
}
