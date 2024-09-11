#include "pch.h"

#include "Platform/Win32/Win32Platform.h"

FWin32Platform::FWin32Platform() = default;

FWin32Platform::~FWin32Platform() = default;

void FWin32Platform::Initialize()
{
}

void FWin32Platform::Shutdown()
{
}

FWin32Platform* GetWin32Platform()
{
	return GetPlatform<FWin32Platform>();
}
