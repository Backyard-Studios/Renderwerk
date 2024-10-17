#pragma once
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions

#include "Renderwerk/DataTypes/Types.h"
#include "Renderwerk/Memory/Memory.h"

template <typename T>
class FSTLAllocator
{
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using size_type = size64;
	using difference_type = std::ptrdiff_t;

	constexpr FSTLAllocator() = default;
	constexpr FSTLAllocator(const FSTLAllocator& Other) noexcept = default;

	template <typename TOther>
	constexpr FSTLAllocator(const FSTLAllocator<TOther>&) noexcept
	{
	}

	constexpr ~FSTLAllocator() = default;

	FSTLAllocator& operator=(const FSTLAllocator& Other) = default;

	pointer allocate(const size_type Count)
	{
		return FMemory::NewArray<value_type>(Count);
	}

	void deallocate(const pointer Pointer, size_type Count) noexcept
	{
		FMemory::DeleteArray(Pointer, Count);
	}

	friend bool operator==(const FSTLAllocator& A, const FSTLAllocator& B) { return &A == &B; }
	friend bool operator!=(const FSTLAllocator& A, const FSTLAllocator& B) { return &A != &B; }

	template <typename TOther>
	struct rebind
	{
		using other = FSTLAllocator<TOther>;
	};

private:
};
