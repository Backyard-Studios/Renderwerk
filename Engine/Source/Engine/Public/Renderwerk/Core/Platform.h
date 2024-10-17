#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
#	define RW_PLATFORM_WINDOWS FORWARD(1)
#	define RW_PLATFORM_NAME "Windows"

#	define RW_PLATFORM_LINUX FORWARD(0)
#elif defined(__linux) || defined(__linux__)
#	define RW_PLATFORM_LINUX FORWARD(1)
#	define RW_PLATFORM_NAME "Linux"

#	define RW_PLATFORM_WINDOWS FORWARD(0)
#endif

// Define TEXT macro if it's not defined by the platform
#if !defined(TEXT) && !RW_PLATFORM_WINDOWS
#	ifdef RW_USE_ANSI_STRINGS
#		define TEXT(x) x
#	else
#		define TEXT(x) L##x
#	endif
#endif
