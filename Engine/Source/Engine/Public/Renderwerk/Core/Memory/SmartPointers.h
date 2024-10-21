#pragma once

#include <memory>
#include <type_traits>

#include "Renderwerk/Core/Memory/Memory.h"

template <class T>
struct FDefaultDelete
{
	CONSTEXPR FDefaultDelete() noexcept = default;

	template <class TOther>
	CONSTEXPR FDefaultDelete(const FDefaultDelete<TOther>&) noexcept requires (std::is_convertible_v<TOther*, T*>)
	{
	}

	CONSTEXPR void operator()(const T* Pointer) const noexcept
	{
		FMemory::Delete(const_cast<T*>(Pointer));
	}
};

/**
 * @brief An object that manages the lifetime of a pointer to an object.
 * @note Unique pointer was renamed into local pointer to make the use case more clear.
 * @tparam T The type of the object that the unique pointer is managing.
 */
template <typename T>
using TLocalPtr = std::unique_ptr<T, FDefaultDelete<T>>;

template <typename T>
using TSharedPtr = std::shared_ptr<T>;

template <typename T>
using TWeakPtr = std::weak_ptr<T>;

template <typename T, typename... TArguments, typename = std::enable_if_t<std::is_constructible_v<T, TArguments...>>>
INLINE TLocalPtr<T> MakeLocal(TArguments&&... Arguments)
{
	T* Pointer = FMemory::New<T>(std::forward<TArguments>(Arguments)...);
	return TLocalPtr<T>(Pointer, FDefaultDelete<T>());
}

template <typename T, typename... TArguments, typename = std::enable_if_t<std::is_constructible_v<T, TArguments...>>>
INLINE TSharedPtr<T> MakeShared(TArguments&&... Arguments)
{
	T* Pointer = FMemory::New<T>(std::forward<TArguments>(Arguments)...);
	return TSharedPtr<T>(Pointer, FDefaultDelete<T>());
}
