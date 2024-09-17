#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <type_traits>
#include <vector>
#include <xutility>

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"

#include "Renderwerk/Memory/Memory.h"
#include "Renderwerk/Memory/SharedPointer.h"
#include "Renderwerk/Memory/UniquePointer.h"
#include "Renderwerk/Memory/WeakPointer.h"

#include "Renderwerk/Platform/Platform.h"

#ifdef RW_PLATFORM_WINDOWS
#	include <Windows.h>
#	include "Renderwerk/Platform/Win32/Win32Platform.h"
#endif
