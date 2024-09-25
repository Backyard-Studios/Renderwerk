#include "pch.h"

#include "Renderwerk/Platform/Launch.h"

#include "Renderwerk/Core/LogManager.h"
#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Platform/Platform.h"

void Launch()
{
	RW_LOG_TRACE("Initializing platform...");
	FPlatform::Initialize();
	RW_LOG_TRACE("Launching engine...");
	GEngine = MakeShared<FEngine>();
	GEngine->Launch();
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
#if RW_PLATFORM_SUPPORTS_SEH
	__try
	{
#endif
		Launch();
#if RW_PLATFORM_SUPPORTS_SEH
	}
	__except (ExceptionHandler(GetExceptionInformation()))
	{
	}
#endif
	Shutdown();
	return FPlatform::GetExitCode();
}

int32 LaunchRenderwerk()
{
	FLogManager::Initialize();
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
