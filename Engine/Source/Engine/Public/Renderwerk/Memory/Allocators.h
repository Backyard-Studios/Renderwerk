#pragma once
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions

#include "Renderwerk/DataTypes/Types.h"
#include "Renderwerk/Memory/Memory.h"

#include <cstddef>

template <typename T>
class FSTLAllocator
{
public:
	using value_type = T;
	using size_type = size64;
	using difference_type = std::ptrdiff_t;

	constexpr FSTLAllocator() noexcept = default;
	constexpr FSTLAllocator(const FSTLAllocator&) noexcept = default;

	template <typename TOther>
	constexpr FSTLAllocator(const FSTLAllocator<TOther>&) noexcept
	{
	}

	constexpr ~FSTLAllocator() = default;

	constexpr FSTLAllocator& operator=(const FSTLAllocator&) = default;

	NODISCARD constexpr value_type* allocate(const size_type Count)
	{
		return static_cast<T*>(FMemory::Allocate(sizeof(T) * Count));
	}

	constexpr void deallocate(value_type* Pointer, size_type Count) noexcept
	{
		FMemory::Free(Pointer);
	}

	template <typename TOther>
	struct rebind
	{
		using other = FSTLAllocator<TOther>;
	};

private:
};

template <class T, class TOther>
bool operator==(const FSTLAllocator<T>&, const FSTLAllocator<TOther>&) { return true; }

template <class T, class TOther>
bool operator!=(const FSTLAllocator<T>&, const FSTLAllocator<TOther>&) { return false; }
