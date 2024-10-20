#pragma once

#include <memory>

#include "Renderwerk/DataTypes/Types.h"
#include "Renderwerk/Memory/Memory.h"

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

template <typename T>
using TUniquePtr = std::unique_ptr<T, FDefaultDelete<T>>;

template <typename T>
using TSharedPtr = std::shared_ptr<T>;

template <typename T>
using TWeakPtr = std::weak_ptr<T>;

template <typename T, typename... TArguments, typename = std::enable_if_t<std::is_constructible_v<T, TArguments...>>>
INLINE TUniquePtr<T> MakeUnique(TArguments&&... Arguments)
{
	T* Pointer = FMemory::New<T>(std::forward<TArguments>(Arguments)...);
	return TUniquePtr<T>(Pointer, FDefaultDelete<T>());
}

template <typename T, typename... TArguments, typename = std::enable_if_t<std::is_constructible_v<T, TArguments...>>>
INLINE TSharedPtr<T> MakeShared(TArguments&&... Arguments)
{
	T* Pointer = FMemory::New<T>(std::forward<TArguments>(Arguments)...);
	return TSharedPtr<T>(Pointer, FDefaultDelete<T>());
}
