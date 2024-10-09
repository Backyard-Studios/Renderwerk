#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#if RW_ENABLE_PROFILING
#	include <tracy/Tracy.hpp>
#endif

#if RW_ENABLE_PROFILING
#	define RW_PROFILING_MARK_FRAME() FrameMark
#	define RW_PROFILING_MARK_FUNCTION() ZoneScoped
#	define RW_PROFILING_MARK_SCOPE(Name) ZoneScopedN(Name)
#	define RW_PROFILING_MARK_THREAD(Name) tracy::SetThreadName(Name)
#else
#	define RW_PROFILING_MARK_FRAME()
#	define RW_PROFILING_MARK_FUNCTION()
#	define RW_PROFILING_MARK_SCOPE(Name)
#	define RW_PROFILING_MARK_THREAD(Name)
#endif
