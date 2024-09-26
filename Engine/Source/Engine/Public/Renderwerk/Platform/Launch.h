#pragma once

#include "Renderwerk/Application/Application.h"
#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Memory/SmartPointers.h"

#if RW_KIND_CONSOLE_APP
#	define PLATFORM_LAUNCH_HEADER int main(int ArgumentCount, char* Arguments[])
#else
#	define PLATFORM_LAUNCH_HEADER int WINAPI WinMain(HINSTANCE InstanceHandle, HINSTANCE PreviousInstanceHandle, LPSTR CommandLine, int ShowCommand)
#endif


ENGINE_API int32 LaunchRenderwerk(const TSharedPtr<IApplication>& Application);

#define LAUNCH_RENDERWERK(Application, ...) \
	PLATFORM_LAUNCH_HEADER \
	{ \
		return LaunchRenderwerk(MakeShared<Application>(__VA_ARGS__)); \
	}
