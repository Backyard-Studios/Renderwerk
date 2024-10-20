#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/DataTypes/String.h"
#include "Renderwerk/DataTypes/Types.h"

#if RW_KIND_CONSOLE_APP
#	ifdef RW_USE_ANSI_STRINGS
#		define ENTRYPOINT_HEADER int main(int32 ArgumentCount, FAnsiChar* Arguments[])
#	else
#		define ENTRYPOINT_HEADER int wmain(int32 ArgumentCount, FWideChar* Arguments[])
#	endif
#elif RW_KIND_WINDOWED_APP
#	include <Windows.h>
#	ifdef RW_USE_ANSI_STRINGS
#		define ENTRYPOINT_HEADER int WINAPI WinMain(HINSTANCE InstanceHandle, HINSTANCE PreviousInstanceHandle, LPSTR CommandLine, int ShowCommand)
#	else
#		define ENTRYPOINT_HEADER int WINAPI wWinMain(HINSTANCE InstanceHandle, HINSTANCE PreviousInstanceHandle, LPWSTR CommandLine, int ShowCommand)
#	endif
#endif

RENDERWERK_API int32 LaunchRenderwerk();

#define LAUNCH_RENDERWERK() \
	ENTRYPOINT_HEADER \
	{ \
		return LaunchRenderwerk(); \
	}
