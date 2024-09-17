#pragma once

#include "Renderwerk/Memory/Memory.h"

template <typename T>
struct FDefaultDeleter
{
public:
	FDefaultDeleter() = default;
	FDefaultDeleter(const FDefaultDeleter& Other) = default;

	template <typename TOther>
	FDefaultDeleter(const FDefaultDeleter<TOther>& Other)
	{
	}

	void operator()(T* Pointer) const
	{
		if (Pointer != nullptr)
			FMemory::Delete(Pointer);
	}
};

/**
 * @brief A unique pointer that owns a single object.
 * The object is automatically deleted when the unique pointer goes out of scope.
 * @tparam T The type of the object to own.
 */
template <typename T, typename TDeleter = FDefaultDeleter<T>>
class TUniquePointer
{
public:
	TUniquePointer() = default;

	TUniquePointer(const std::nullptr_t InNullptr)
		: Pointer(InNullptr)
	{
	}

	TUniquePointer(T* InPointer)
		: Pointer(InPointer)
	{
	}

	TUniquePointer(T* InPointer, TDeleter InDeleter)
		: Pointer(InPointer), Deleter(InDeleter)
	{
	}

	TUniquePointer(const TUniquePointer& Other) = delete;

	TUniquePointer(TUniquePointer&& Other) noexcept
		: Pointer(Other.Release()), Deleter(Other.Deleter)
	{
	}

	template <typename TOther>
	TUniquePointer(TUniquePointer<TOther>& Other) = delete;

	template <typename TOther>
	TUniquePointer(TUniquePointer<TOther>&& Other) noexcept
		: Pointer(Other.Release()), Deleter(Other.Deleter)
	{
	}

	~TUniquePointer()
	{
		Reset();
	}

public:
	TUniquePointer& operator=(TUniquePointer& Other) = delete;

	TUniquePointer& operator=(TUniquePointer&& Other) noexcept
	{
		TUniquePointer Temp(std::move(Other));
		Temp.Swap(*this);
		return *this;
	}

	template <typename TOther>
	TUniquePointer& operator=(TUniquePointer<TOther>& Other) = delete;

	template <typename TOther>
	TUniquePointer& operator=(TUniquePointer<TOther>&& Other) noexcept
	{
		TUniquePointer<TOther> Temp(std::move(Other));
		Temp.Swap(*this);
		return *this;
	}

	operator bool() { return Pointer != nullptr; }
	operator bool() const { return Pointer != nullptr; }

	T* operator->() { return Pointer; }
	const T* operator->() const { return Pointer; }

	T& operator*() { return *Pointer; }
	const T& operator*() const { return *Pointer; }

	bool operator==(const TUniquePointer& Other) const { return Pointer == Other.Pointer; }
	bool operator!=(const TUniquePointer& Other) const { return Pointer != Other.Pointer; }

public:
	/**
	 * @brief Reset the unique pointer with a new instance or nullptr.
	 * @param NewInstance The new instance to own.
	 */
	void Reset(T* NewInstance = nullptr)
	{
		if (Pointer != nullptr)
			Deleter(Pointer);
		Pointer = NewInstance;
	}

	T* Release()
	{
		T* TempPointer = Pointer;
		Pointer = nullptr;
		return TempPointer;
	}

	void Swap(const TUniquePointer& Other)
	{
		std::swap(Pointer, Other.Pointer);
		std::swap(Deleter, Other.Deleter);
	}

	[[nodiscard]] bool IsValid() const { return Pointer != nullptr; }

public:
	[[nodiscard]] T* GetRaw() { return Pointer; }
	[[nodiscard]] const T* GetRaw() const { return Pointer; }

private:
	T* Pointer = nullptr;
	TDeleter Deleter;
};

/**
 * @brief Create a new unique pointer.
 * @tparam T The type of the object to create.
 * @tparam TArguments The types of the arguments to pass to the constructor.
 * @param Arguments The arguments to pass to the constructor.
 * @return The unique pointer.
 */
template <typename T, typename... TArguments>
[[nodiscard]] TUniquePointer<T> MakeUnique(TArguments&&... Arguments)
{
	T* Instance = FMemory::New<T>(std::forward<TArguments>(Arguments)...);
	return TUniquePointer<T>(Instance);
}
