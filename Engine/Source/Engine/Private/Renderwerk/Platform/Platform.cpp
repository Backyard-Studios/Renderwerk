#include "pch.h"

#include "Renderwerk/Platform/Platform.h"

#include <cassert>

TSharedPointer<IPlatform> GPlatform = nullptr;

TSharedPointer<IPlatform> GetPlatform()
{
	assert(GPlatform != nullptr);
	return GPlatform;
}
