#include "pch.h"

#include "Renderwerk/Core/Result.h"

#include "Renderwerk/Platform/Launch.h"

#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Platform/Platform.h"

#if RW_PLATFORM_WINDOWS
#	include "Renderwerk/Platform/Win32/Win32Platform.h"
#endif

int32 LaunchRenderwerk()
{
	RegisterDefaultResultDescriptions();

#if RW_PLATFORM_WINDOWS
	GPlatform = MakeShared<FWin32Platform>();
#endif
	FResult Result = GPlatform->Initialize();
	if (Result.IsError())
		return Result.GetErrorCode();

	GEngine = MakeShared<FEngine>();
	Result = GEngine->Launch();
	if (Result.IsError())
		return Result.GetErrorCode();

	GEngine.Reset();
	GPlatform.Reset();
	return 0;
}
