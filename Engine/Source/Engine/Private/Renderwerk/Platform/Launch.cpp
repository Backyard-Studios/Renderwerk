#include "pch.h"

#include "Renderwerk/Core/Result.h"

#include "Renderwerk/Platform/Launch.h"

#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Platform/Platform.h"

#if RW_PLATFORM_WINDOWS
#	include "Renderwerk/Platform/Win32/Win32Platform.h"
#endif

int32 Launch()
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
	return 0;
}

void Shutdown()
{
	if (GEngine)
	{
		GEngine->Shutdown();
		GEngine.Reset();
	}
	if (GPlatform)
	{
		GPlatform->Shutdown();
		GPlatform.Reset();
	}
}

int32 GuardedMain()
{
#if RW_PLATFORM_SUPPORTS_SEH
	__try
	{
#endif
		return Launch();
#if RW_PLATFORM_SUPPORTS_SEH
	}
	__except (ExceptionHandler(GetExceptionInformation()))
	{
	}
#endif
	Shutdown();
	return IPlatform::GetExitCode();
}

int32 LaunchRenderwerk()
{
	return GuardedMain();
}
