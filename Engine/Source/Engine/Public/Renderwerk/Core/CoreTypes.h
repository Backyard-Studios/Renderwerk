#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <vector>

#pragma region Primitive Types

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using byte = uint8;

using float32 = float;
using float64 = double;

using bool8 = bool;

using size64 = size_t;

using char8 = char;
using char16 = char16_t;
using char32 = char32_t;

using wchar = wchar_t;

#pragma endregion

#pragma region Containers
// Only temporary until we have proper implementations

template <typename T>
using TVector = std::vector<T>;

template <typename TKey, typename TValue>
using TMap = std::map<TKey, TValue>;

template <typename T>
using TSet = std::set<T>;

#pragma endregion

#pragma region Smart Pointers
// Only temporary until we have proper implementations

template <typename T>
using TWeakPtr = std::weak_ptr<T>;

#pragma endregion

template <typename T>
using TFunction = std::function<T>;
