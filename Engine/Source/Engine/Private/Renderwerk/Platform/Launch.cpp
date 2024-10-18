#include "pch.h"

#include "Renderwerk/Platform/Launch.h"
#include "Renderwerk/Platform/ExceptionHandling.h"

void GuardedMain()
{
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
