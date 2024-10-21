#pragma once

#include <type_traits>

#include "Renderwerk/Core/CoreMinimal.h"

#if RW_ENABLE_MEMORY_TRACKING

struct RENDERWERK_API FMemoryStatistics
{
	size64 CurrentUsage = 0;
};

#endif

#define RW_DEFAULT_MEMORY_ALIGNMENT FORWARD(16)

class RENDERWERK_API FMemory
{
public:
	NODISCARD static void* Allocate(size64 Size, size64 Alignment = RW_DEFAULT_MEMORY_ALIGNMENT);
	static void Free(void* Memory);

	NODISCARD static void* Reallocate(void* Memory, size64 Size, size64 Alignment = RW_DEFAULT_MEMORY_ALIGNMENT);
	static void Copy(void* Destination, const void* Source, size64 Size);

	NODISCARD static size64 GetSizeOfMemory(const void* Memory);
	NODISCARD static size64 CalculateAlignedSize(size64 Size, size64 Alignment);

	template <typename T, typename... TArguments, typename = std::enable_if_t<std::is_constructible_v<T, TArguments...>>>
	NODISCARD static T* New(TArguments&&... Arguments)
	{
		return NewAligned<T>(RW_DEFAULT_MEMORY_ALIGNMENT, std::forward<TArguments>(Arguments)...);
	}

	template <typename T, typename... TArguments, typename = std::enable_if_t<std::is_constructible_v<T, TArguments...>>>
	NODISCARD static T* NewAligned(const size64 Alignment, TArguments&&... Arguments)
	{
		void* Memory = Allocate(sizeof(T), Alignment);
		return new(Memory) T(std::forward<TArguments>(Arguments)...);
	}

	template <typename T>
	static void Delete(T* Object)
	{
		Object->~T();
		Free(Object);
	}

	template <typename T, typename... TArguments, typename = std::enable_if_t<std::is_constructible_v<T, TArguments...>>>
	NODISCARD static T* NewArray(const size64 Count, TArguments&&... Arguments)
	{
		return NewArrayAligned<T>(Count, RW_DEFAULT_MEMORY_ALIGNMENT, std::forward<TArguments>(Arguments)...);
	}

	template <typename T, typename... TArguments, typename = std::enable_if_t<std::is_constructible_v<T, TArguments...>>>
	NODISCARD static T* NewArrayAligned(const size64 Count, const size64 Alignment, TArguments&&... Arguments)
	{
		const size64 Size = sizeof(T) * Count;
		void* Memory = Allocate(Size, Alignment);
		T* Array = static_cast<T*>(Memory);
		for (size64 Index = 0; Index < Count; ++Index)
			new(Array + Index) T(std::forward<TArguments>(Arguments)...);
		return Array;
	}

	template <typename T>
	static void DeleteArray(T* Array, const size64 Count)
	{
		for (size64 Index = 0; Index < Count; ++Index)
			std::destroy_at(Array + Index);
		Free(Array);
	}

public:
#if RW_ENABLE_MEMORY_TRACKING
	NODISCARD static FMemoryStatistics* GetMemoryStatistics();
#endif

private:
#if RW_ENABLE_MEMORY_TRACKING
	static FMemoryStatistics* MemoryStatistics;
#endif
};
