#pragma once

#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
#	define RW_PLATFORM_WINDOWS FORWARD(1)
#	define RW_PLATFORM_NAME "Windows"

#	define RW_PLATFORM_LINUX FORWARD(0)
#elif defined(__linux) || defined(__linux__)
#	define RW_PLATFORM_LINUX FORWARD(1)
#	define RW_PLATFORM_NAME "Linux"

#	define RW_PLATFORM_WINDOWS FORWARD(0)
#endif

#if RW_PLATFORM_WINDOWS
#	include <Windows.h>
#endif

// Define TEXT macro if it's not defined by the platform
#ifndef TEXT
#	ifdef RW_USE_WIDE_STRINGS_AS_DEFAULT
#		define TEXT(x) L##x
#	else
#		define TEXT(x) x
#	endif
#endif

#if RW_PLATFORM_WINDOWS
#	define RW_PLATFORM_SUPPORTS_SEH FORWARD(1)
#else
#	define RW_PLATFORM_SUPPORTS_SEH FORWARD(0)
#endif
