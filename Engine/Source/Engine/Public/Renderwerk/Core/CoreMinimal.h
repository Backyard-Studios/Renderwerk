#pragma once

#include "Renderwerk/Core/Compiler.h"
#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Core/Platform.h"

#if RW_COMPILE_ENGINE_API
#	define ENGINE_API DLL_EXPORT
#else
#	define ENGINE_API DLL_IMPORT
#endif
