#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"

#if RW_PLATFORM_WINDOWS
#	include "Renderwerk/Platform/Win32/Win32Launch.h"
#endif

#ifndef PLATFORM_LAUNCH_HEADER
#	error "PLATFORM_LAUNCH_HEADER must be defined"
#endif

ENGINE_API int32 LaunchRenderwerk();

#define LAUNCH_RENDERWERK() \
	PLATFORM_LAUNCH_HEADER \
	{ \
		return LaunchRenderwerk(); \
	}
