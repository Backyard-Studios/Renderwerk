#pragma once

// Platfor detection
#if defined(_WIN32) || defined(__WIN32__)
#	ifndef _WIN64
#		error "Only 64-bit Windows is supported"
#	endif
#	define RW_PLATFORM "Windows"
#	define RW_PLATFORM_WINDOWS 1
#	define RW_PLATFORM_LINUX 0
#elif defined(__linux__) || defined(__gnu_linux__)
#	define RW_PLATFORM "Linux"
#	define RW_PLATFORM_LINUX 1
#	define RW_PLATFORM_WINDOWS 0
#else
#	error "Unsupported platform"
#endif

// Compiler detection
#if defined(_MSC_VER)
#	define RW_COMPILER_MSVC 1
#	define RW_COMPILER_GCC 0
#elif defined(__GNUC__)
#	define RW_COMPILER_GCC 1
#	define RW_COMPILER_MSVC 0
#else
#	error "Unsupported compiler"
#endif

#if RW_PLATFORM_WINDOWS
#	include <Windows.h>
#endif

// If there is no definition for TEXT, define it
#ifndef TEXT
#	if RW_PLATFORM_LINUX
#		define RW_TEXT_CHAR16 1
#	else
#		define RW_TEXT_CHAR16 0
#	endif
#	if RW_TEXT_CHAR16
#		define __TEXT(x) u##x
#	else
#		define __TEXT(x) L##x
#	endif
#	define TEXT(x) __TEXT(x)
#endif

#if RW_COMPILER_MSVC
#	define DLL_EXPORT __declspec(dllexport)
#	define DLL_IMPORT __declspec(dllimport)
#	define INLINE __inline
#	define FORCEINLINE __forceinline
#	define NOINLINE __declspec(noinline)
#	define DEPRECATED __declspec(deprecated)
#	define DEPRECATED_MSG(msg) __declspec(deprecated(msg))
#else
#	define DLL_EXPORT __attribute__((visibility("default")))
#	define DLL_IMPORT
#	define INLINE inline
#	define FORCEINLINE inline
#	define NOINLINE __attribute__((noinline))
#	define DEPRECATED __attribute__((deprecated))
#	define DEPRECATED_MSG(msg) __attribute__((deprecated(msg)))
#endif

#if RW_COMPILE_ENGINE_API
#	define ENGINE_API DLL_EXPORT
#else
#	define ENGINE_API DLL_IMPORT
#endif
