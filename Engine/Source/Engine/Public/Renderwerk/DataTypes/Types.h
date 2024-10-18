#pragma once

#include <cstdint>
#include <functional>
#include <source_location>

#pragma region Primitive Types

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using float32 = float;
using float64 = double;

using byte = uint8;

using bool8 = bool;

using size64 = uint64;

#pragma endregion

template <typename T>
using TFunction = std::function<T>;

using FSourceLocation = std::source_location;
