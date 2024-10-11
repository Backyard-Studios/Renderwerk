#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <queue>
#include <ranges>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <xutility>

#include "Renderwerk/Core/Assertion.h"
#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Core/LogManager.h"
#include "Renderwerk/Core/StringUtils.h"
#include "Renderwerk/Core/Version.h"

#include "Renderwerk/Debug/Profiling.h"

#include "Renderwerk/Memory/Memory.h"
#include "Renderwerk/Memory/SmartPointers.h"

#include "Renderwerk/Platform/Directories.h"
#include "Renderwerk/Platform/Guid.h"
#include "Renderwerk/Platform/Platform.h"
#include "Renderwerk/Platform/Threading/Mutex.h"
#include "Renderwerk/Platform/Threading/Signal.h"
#include "Renderwerk/Platform/Threading/Thread.h"

#ifdef RW_PLATFORM_WINDOWS
#	include <Windows.h>
#endif
