#include "pch.h"

#include "Renderwerk/Platform/Launch.h"
#include "Renderwerk/Platform/ExceptionHandling.h"
#include "Renderwerk/Platform/Platform.h"

void GuardedMain()
{
	GPlatform = MakeShared<FPlatform>();

	GPlatform.reset();
}

int32 LaunchRenderwerk()
{
	__try
	{
		GuardedMain();
	}
	__except (FExceptionHandling::Handler(GetExceptionInformation()))
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
