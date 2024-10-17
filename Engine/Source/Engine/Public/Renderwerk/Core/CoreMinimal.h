#pragma once

#include "Renderwerk/Core/Compiler.h"
#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Core/Platform.h"

// []===================================================================[]
// The macro RW_LIBRARY_SHARED is a user defined macro that is used
// to determine if the library is being built as a shared library or as
// a static library. If it is defined the RENDERWERK_API macro will be set
// to DLL_EXPORT if RW_COMPILE_LIBRARY is defined, otherwise it will
// be set to DLL_IMPORT. If RW_LIBRARY_SHARED is not defined, the
// RENDERWERK_API macro will be set to nothing.
// []===================================================================[]
#ifdef RW_LIBRARY_SHARED
#	if RW_COMPILE_LIBRARY
		#define RENDERWERK_API DLL_EXPORT
#	else
		#define RENDERWERK_API DLL_IMPORT
#	endif
#else
#	define RENDERWERK_API
#endif
