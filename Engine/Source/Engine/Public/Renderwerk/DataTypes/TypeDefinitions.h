#pragma once

#include <array>
#include <cstdint>
#include <deque>
#include <functional>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

#pragma region String Types

using FAnsiString = std::string;
using FAnsiStringView = std::string_view;
using FAnsiChar = char;

using FWideString = std::wstring;
using FWideStringView = std::wstring_view;
using FWideChar = wchar_t;

#ifdef RW_USE_ANSI_STRINGS
using FString = FAnsiString;
using FStringView = FAnsiStringView;
using FChar = FAnsiChar;
#else
using FString = FWideString;
using FStringView = FWideStringView;
using FChar = FWideChar;
#endif

#pragma endregion

#pragma region Container Types

template <typename T, size64 TSize>
using TArray = std::array<T, TSize>;

template <typename T>
using TVector = std::vector<T>;

template <typename TKey, typename TValue>
using TMap = std::map<TKey, TValue>;

template <typename TKey, typename TValue>
using TUnorderedMap = std::unordered_map<TKey, TValue>;

template <typename T>
using TSet = std::set<T>;

template <typename T>
using TUnorderedSet = std::unordered_set<T>;

template <typename T>
using TQueue = std::queue<T>;

template <typename T>
using TDeque = std::deque<T>;

#pragma endregion

#pragma region Function Types

template <typename T>
using TFunction = std::function<T>;

#pragma endregion
