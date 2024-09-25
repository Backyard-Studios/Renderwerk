#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"

#include <string>

using FVersion = uint32;

#define RW_MAKE_VERSION(Major, Minor, Patch) \
	((Major << 22) | (Minor << 12) | Patch)

#define RW_VERSION_MAJOR(Version) \
	((Version >> 22) & 0x3FF)

#define RW_VERSION_MINOR(Version) \
	((Version >> 12) & 0x3FF)

#define RW_VERSION_PATCH(Version) \
	(Version & 0xFFF)

ENGINE_API INLINE std::string FormatVersion(const FVersion Version)
{
	return std::to_string(RW_VERSION_MAJOR(Version)) + "." + std::to_string(RW_VERSION_MINOR(Version)) + "." + std::to_string(RW_VERSION_PATCH(Version));
}
