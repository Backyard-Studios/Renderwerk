#include "pch.h"

#include "Renderwerk/Platform/Launch.h"

#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Platform/ExceptionHandling.h"
#include "Renderwerk/Platform/Platform.h"

void GuardedMain()
{
	RW_LOG(LogDefault, Info, "{} {} by {}", TEXT(RW_ENGINE_NAME), TEXT(RW_ENGINE_FULL_VERSION), TEXT(RW_ENGINE_AUTHOR));

	GPlatform = MakeShared<FPlatform>();
	RW_LOG(LogDefault, Info, "CPU Information:");
	RW_LOG(LogDefault, Info, "\t- Name: {}", GetPlatform()->GetProcessorInfo().Name);
	RW_LOG(LogDefault, Info, "\t- Physical Cores: {}", GetPlatform()->GetProcessorInfo().PhysicalCoreCount);
	RW_LOG(LogDefault, Info, "\t- Logical Cores: {}", GetPlatform()->GetProcessorInfo().LogicalCoreCount);
	RW_LOG(LogDefault, Info, "\t- x64 Architecture: {}", GetPlatform()->GetProcessorInfo().bIs64Bit);

	RW_LOG(LogDefault, Info, "Memory Information:");
	RW_LOG(LogDefault, Info, "\t- Total Physical Memory: {} Gb", RW_GIBIBYTE(GetPlatform()->GetMemoryInfo().TotalPhysicalMemory));
	GEngine = MakeShared<FEngine>();

	GEngine.reset();
	GPlatform.reset();
}

int32 LaunchRenderwerk()
{
	FLogManager::Initialize();
#ifdef RW_CONFIG_DEBUG
	// Enable heap corruption detection. This will cause the application to crash if the heap is corrupted.
	HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);

	// Enable memory leak detection. This will cause the application to report memory leaks after it has exited.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Disables the display of the message box when a CRT assertion fails.
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
#endif
	__try
	{
		GuardedMain();
	}
	__except (FExceptionHandling::Handler(GetExceptionInformation()))
	{
		FLogManager::Shutdown();
		return EXIT_FAILURE;
	}
	FLogManager::Shutdown();
	return EXIT_SUCCESS;
}
