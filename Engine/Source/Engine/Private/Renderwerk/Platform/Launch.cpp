#include "pch.h"

#include "Renderwerk/Platform/Launch.h"

#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Platform/ExceptionHandling.h"
#include "Renderwerk/Platform/Platform.h"

void GuardedMain()
{
	GPlatform = MakeShared<FPlatform>();
	GEngine = MakeShared<FEngine>();

	GEngine.reset();
	GPlatform.reset();
}

int32 LaunchRenderwerk()
{
	FLogManager::Initialize();
	// __try
	// {
	GuardedMain();
	// }
	// __except (FExceptionHandling::Handler(GetExceptionInformation()))
	// {
	// 	FLogManager::Shutdown();
	// 	return EXIT_FAILURE;
	// }
	FLogManager::Shutdown();
	return EXIT_SUCCESS;
}
