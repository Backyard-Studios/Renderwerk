#include "pch.h"

#include "Renderwerk/Core/Result.h"

#include "Renderwerk/Platform/Launch.h"

#include "Renderwerk/Core/LogManager.h"
#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Platform/Platform.h"

#if RW_PLATFORM_WINDOWS
#	include "Renderwerk/Platform/Win32/Win32Platform.h"
#endif

int32 Launch()
{
	RegisterDefaultResultDescriptions();

	RW_LOG_TRACE("Initializing platform...");
#if RW_PLATFORM_WINDOWS
	GPlatform = MakeShared<FWin32Platform>();
#endif
	FResult Result = GPlatform->Initialize();
	if (Result.IsError())
	{
		RW_LOG_ERROR("Failed to initialize platform: {}", Result.GetReason());
		return Result.GetErrorCode();
	}

	RW_LOG_TRACE("Launching engine...");
	GEngine = MakeShared<FEngine>();
	Result = GEngine->Launch();
	if (Result.IsError())
		return Result.GetErrorCode();
	return 0;
}

void Shutdown()
{
	if (IPlatform::GetExitCode() != 0)
		RW_LOG_ERROR("Trying to shutdown gracefully after an error occurred");

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
	RW_LOG_INFO("Successfully shut down");
}

int32 GuardedMain()
{
	int32 ExitCode = 0;
#if RW_PLATFORM_SUPPORTS_SEH
	__try
	{
#endif
		ExitCode = Launch();
#if RW_PLATFORM_SUPPORTS_SEH
	}
	__except (ExceptionHandler(GetExceptionInformation()))
	{
	}
#endif
	if (IPlatform::GetExitCode() == 0)
		IPlatform::SetExitCode(ExitCode);
	Shutdown();
	return IPlatform::GetExitCode();
}

int32 LaunchRenderwerk()
{
	FResult Result = FLogManager::Initialize();
	if (Result.IsError())
		return Result.GetErrorCode();
#if RW_ENABLE_MEMORY_TRACKING
	RW_LOG_DEBUG("Memory tracking enabled");
#endif
#if RW_ENABLE_PROFILING
	RW_LOG_DEBUG("Profiling enabled");
#endif
#if RW_ENABLE_PROFILING && RW_ENABLE_MEMORY_TRACKING
	FMemory::GetMemoryTracking().SetOnAllocateCallback([](const void* Pointer, const size64 Size, uint8 Alignment)
	{
		TracyAlloc(Pointer, Size);
	});
	FMemory::GetMemoryTracking().SetOnFreeCallback([](const void* Pointer, uint8 Alignment)
	{
		TracyFree(Pointer);
	});
#endif
	int32 ExitCode = GuardedMain();
	FLogManager::Shutdown();
	return ExitCode;
}
