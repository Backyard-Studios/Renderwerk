#include "pch.h"

#include "Renderwerk/Platform/Launch.h"

#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Platform/Platform.h"

#if RW_PLATFORM_WINDOWS
#	include "Renderwerk/Platform/Win32/Win32Platform.h"
#endif

int32 LaunchRenderwerk()
{
#if RW_PLATFORM_WINDOWS
	GPlatform = MakeShared<FWin32Platform>();
#endif

	GEngine = MakeShared<FEngine>();
	GEngine->Launch();

	GEngine.Reset();
	GPlatform.Reset();
	return 0;
}
