#include "pch.h"

#include "Renderwerk/Core/Result.h"

#include "Renderwerk/Platform/Launch.h"

#include "Renderwerk/Core/LogManager.h"
#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Platform/Platform.h"

int32 Launch()
{
	RW_LOG_TRACE("Initializing platform...");
	FResult Result = FPlatform::Initialize();
	if (Result.IsError())
	{
		RW_LOG_ERROR("Failed to initialize platform: {}", Result.GetReason());
		return Result.GetCode();
	}

	RW_LOG_TRACE("Launching engine...");
	GEngine = MakeShared<FEngine>();
	Result = GEngine->Launch();
	if (Result.IsError())
		return Result.GetCode();
	return 0;
}

void Shutdown()
{
	if (FPlatform::GetExitCode() != 0)
		RW_LOG_ERROR("Trying to shutdown gracefully after an error occurred");

	if (GEngine)
	{
		GEngine->Shutdown();
		GEngine.Reset();
	}
	FPlatform::Shutdown();
	RW_LOG_INFO("Successfully shut down");
}

int32 GuardedMain()
{
	int32 ExitCode = 0;
	// #if RW_PLATFORM_SUPPORTS_SEH
	// 	__try
	// 	{
	// #endif
	ExitCode = Launch();
	// #if RW_PLATFORM_SUPPORTS_SEH
	// 	}
	// 	__except (ExceptionHandler(GetExceptionInformation()))
	// 	{
	// 	}
	// #endif
	if (FPlatform::GetExitCode() == 0)
		FPlatform::SetExitCode(ExitCode);
	Shutdown();
	return FPlatform::GetExitCode();
}

int32 LaunchRenderwerk()
{
	FResult Result = FLogManager::Initialize();
	if (Result.IsError())
		return Result.GetCode();
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
