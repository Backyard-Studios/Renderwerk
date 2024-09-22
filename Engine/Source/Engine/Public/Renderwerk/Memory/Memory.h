#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"

#include <type_traits>
#include <xutility>

enum : uint8
{
	MEMORY_DEFAULT_ALIGNMENT = 16
};

#if RW_ENABLE_MEMORY_TRACKING
struct ENGINE_API FMemoryTracking
{
	using FOnAllocateCallback = TFunction<void(void* Pointer, size64 Size, uint8 Alignment)>;
	using FOnFreeCallback = TFunction<void(void* Pointer, uint8 Alignment)>;

public:
	void SetOnAllocateCallback(const FOnAllocateCallback& Callback);
	void SetOnFreeCallback(const FOnFreeCallback& Callback);

	void OnAllocate(void* Pointer, size64 Size, uint8 Alignment);
	void OnFree(void* Pointer, uint8 Alignment);

public:
	[[nodiscard]] size64 GetUsage() const { return Usage; }
	[[nodiscard]] size64 GetPeakUsage() const { return PeakUsage; }

private:
	size64 Usage = 0;
	size64 PeakUsage = 0;

	FOnAllocateCallback OnAllocateCallback;
	FOnFreeCallback OnFreeCallback;
};
#endif

class ENGINE_API FMemory
{
public:
	static void* Allocate(const size64 Size, const uint8 Alignment = MEMORY_DEFAULT_ALIGNMENT)
	{
		void* Pointer = _aligned_malloc(Size, Alignment);
#if RW_ENABLE_MEMORY_TRACKING
		GetMemoryTracking().OnAllocate(Pointer, Size, Alignment);
#endif
		return Pointer;
	}

	static void Free(void* Pointer, const uint8 Alignment = MEMORY_DEFAULT_ALIGNMENT)
	{
#if RW_ENABLE_MEMORY_TRACKING
		GetMemoryTracking().OnFree(Pointer, Alignment);
#endif
		_aligned_free(Pointer);
	}

	static void Zero(void* Pointer, const size64 Size)
	{
		ZeroMemory(Pointer, Size);
	}

	static void Copy(void* Destination, const void* Source, const size64 Size)
	{
		CopyMemory(Destination, Source, Size);
	}

	static void Fill(void* Destination, const uint8 Value, const size64 Size)
	{
		FillMemory(Destination, Size, Value);
	}

	static void Move(void* Destination, const void* Source, const size64 Size)
	{
		MoveMemory(Destination, Source, Size);
	}

	template <typename T, typename... TArguments>
	static T* New(TArguments&&... Arguments)
	{
		return NewAligned<T>(MEMORY_DEFAULT_ALIGNMENT, std::forward<TArguments>(Arguments)...);
	}

	template <typename T, typename... TArguments>
	static T* NewAligned(const uint8 Alignment, TArguments&&... Arguments)
	{
		void* Memory = Allocate(sizeof(T), Alignment);
		return std::construct_at(static_cast<T*>(Memory), std::forward<TArguments>(Arguments)...);
	}

	template <typename T>
	static void Delete(T* Object)
	{
		std::destroy_at(Object);
		Free(Object);
	}

	template <typename T, typename... TArguments>
	static T* NewArray(const size64 Count, TArguments&&... Arguments)
	{
		return NewArrayAligned<T>(Count, MEMORY_DEFAULT_ALIGNMENT, std::forward<TArguments>(Arguments)...);
	}

	template <typename T, typename... TArguments>
	static T* NewArrayAligned(const size64 Count, const uint8 Alignment, TArguments&&... Arguments)
	{
		const size64 Size = sizeof(T) * Count;
		void* Memory = Allocate(Size, Alignment);
		T* Array = static_cast<T*>(Memory);
		for (size64 Index = 0; Index < Count; ++Index)
			std::construct_at(Array + Index, std::forward<TArguments>(Arguments)...);
		return Array;
	}

	template <typename T>
	static void DeleteArray(T* Array, const size64 Count)
	{
		for (size64 Index = 0; Index < Count; ++Index)
			std::destroy_at(Array + Index);
		Free(Array);
	}


#if RW_ENABLE_MEMORY_TRACKING
	static FMemoryTracking& GetMemoryTracking()
	{
		static FMemoryTracking MemoryTracking;
		return MemoryTracking;
	}
#endif

private:
};
