#pragma once
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions

#include "Renderwerk/Core/Compiler.h"
#include "Renderwerk/Core/Memory/Memory.h"
#include "Renderwerk/Core/Types/Types.h"

#include <cstddef>

template <typename T>
class FSTLAllocator
{
public:
	using value_type = T;
	using size_type = size64;
	using difference_type = std::ptrdiff_t;

	CONSTEXPR FSTLAllocator() noexcept = default;
	CONSTEXPR FSTLAllocator(const FSTLAllocator&) noexcept = default;

	template <typename TOther>
	CONSTEXPR FSTLAllocator(const FSTLAllocator<TOther>&) noexcept
	{
	}

	CONSTEXPR ~FSTLAllocator() = default;

	CONSTEXPR FSTLAllocator& operator=(const FSTLAllocator&) = default;

	NODISCARD CONSTEXPR value_type* allocate(const size_type Count)
	{
		return static_cast<T*>(FMemory::Allocate(sizeof(T) * Count));
	}

	CONSTEXPR void deallocate(value_type* Pointer, size_type Count) noexcept
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
