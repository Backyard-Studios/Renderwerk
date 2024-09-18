#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <ranges>
#include <set>
#include <string>
#include <type_traits>
#include <vector>
#include <xutility>

#include "Renderwerk/Core/Assertion.h"
#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Core/Result.h"
#include "Renderwerk/Core/StringUtils.h"

#include "Renderwerk/Debug/Profiling.h"

#include "Renderwerk/Memory/Memory.h"
#include "Renderwerk/Memory/SharedPointer.h"
#include "Renderwerk/Memory/UniquePointer.h"
#include "Renderwerk/Memory/WeakPointer.h"

#include "Renderwerk/Platform/Guid.h"
#include "Renderwerk/Platform/Platform.h"

#ifdef RW_PLATFORM_WINDOWS
#	include <Windows.h>
#	include "Renderwerk/Platform/Win32/Win32Platform.h"
#endif
