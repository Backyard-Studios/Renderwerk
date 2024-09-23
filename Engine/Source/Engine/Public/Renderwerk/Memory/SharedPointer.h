#pragma once

#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Memory/UniquePointer.h"

#include <atomic>

template <typename T, typename TDeleter = FDefaultDeleter<T>>
class TSharedPointer
{
	template <typename TOther, typename TOtherDeleter>
	friend class TSharedPointer;

public:
	TSharedPointer()
		: Pointer(nullptr), ReferenceCount(FMemory::New<std::atomic<uint32>>(1))
	{
	}

	TSharedPointer(const std::nullptr_t InNullptr)
		: Pointer(InNullptr), ReferenceCount(FMemory::New<std::atomic<uint32>>(1))
	{
	}

	TSharedPointer(T* InPointer)
		: Pointer(InPointer), ReferenceCount(FMemory::New<std::atomic<uint32>>(1))
	{
	}

	TSharedPointer(T* InPointer, TDeleter InDeleter)
		: Pointer(InPointer), Deleter(InDeleter), ReferenceCount(FMemory::New<std::atomic<uint32>>(1))
	{
	}

	TSharedPointer(const TSharedPointer& Other)
		: Pointer(Other.Pointer), Deleter(Other.Deleter), ReferenceCount(Other.ReferenceCount)
	{
		IncrementReferenceCount();
	}

	TSharedPointer(TSharedPointer&& Other) noexcept
		: Pointer(Other.Pointer), Deleter(Other.Deleter), ReferenceCount(Other.ReferenceCount)
	{
		Other.Pointer = nullptr;
		Other.ReferenceCount = nullptr;
	}

	template <typename TOther>
	TSharedPointer(TSharedPointer<TOther>& Other)
		: Pointer(static_cast<T*>(Other.Pointer)), Deleter(Other.Deleter), ReferenceCount(Other.ReferenceCount)
	{
		IncrementReferenceCount();
	}

	template <typename TOther>
	TSharedPointer(TSharedPointer<TOther>&& Other) noexcept
		: Pointer(static_cast<T*>(Other.Pointer)), Deleter(Other.Deleter), ReferenceCount(Other.ReferenceCount)
	{
		Other.Pointer = nullptr;
		Other.ReferenceCount = nullptr;
	}

	~TSharedPointer()
	{
		DecrementReferenceCount();
	}

public:
	TSharedPointer& operator=(const TSharedPointer& Other)
	{
		TSharedPointer Temp(Other);
		Temp.Swap(*this);
		return *this;
	}

	TSharedPointer& operator=(TSharedPointer&& Other) noexcept
	{
		TSharedPointer Temp(std::move(Other));
		Temp.Swap(*this);
		return *this;
	}

	operator bool() { return Pointer != nullptr; }
	operator bool() const { return Pointer != nullptr; }

	T* operator->() { return Pointer; }
	const T* operator->() const { return Pointer; }

	T& operator*() { return *Pointer; }
	const T& operator*() const { return *Pointer; }

	bool operator==(const TSharedPointer& Other) const { return Pointer == Other.Pointer; }
	bool operator!=(const TSharedPointer& Other) const { return Pointer != Other.Pointer; }

public:
	void Reset(T* NewInstance = nullptr)
	{
		DecrementReferenceCount();
		Pointer = NewInstance;
		IncrementReferenceCount();
	}

	void Swap(TSharedPointer& Other)
	{
		std::swap(Pointer, Other.Pointer);
		std::swap(Deleter, Other.Deleter);
		std::swap(ReferenceCount, Other.ReferenceCount);
	}

	template <typename TOther>
	[[nodiscard]] TSharedPointer<TOther> As()
	{
		return TSharedPointer<TOther>(*this);
	}

	[[nodiscard]] bool IsValid() const { return Pointer != nullptr; }

public:
	[[nodiscard]] T* GetRaw() { return Pointer; }
	[[nodiscard]] const T* GetRaw() const { return Pointer; }

	[[nodiscard]] uint32 GetReferenceCount() const { return ReferenceCount->load(); }

private:
	void IncrementReferenceCount()
	{
		if (Pointer)
			++ReferenceCount;
	}

	void DecrementReferenceCount()
	{
		if (ReferenceCount == nullptr)
			return;
		--ReferenceCount;
		if (GetReferenceCount() == 0)
		{
			if (Pointer)
				Deleter(Pointer);
			Pointer = nullptr;

			FMemory::Delete(ReferenceCount);
			ReferenceCount = nullptr;
		}
	}

private:
	T* Pointer;
	TDeleter Deleter;

	std::atomic<uint32>* ReferenceCount;
};

template <typename T, typename... TArguments>
[[nodiscard]] TSharedPointer<T> MakeShared(TArguments&&... Arguments)
{
	T* Instance = FMemory::New<T>(std::forward<TArguments>(Arguments)...);
	return TSharedPointer<T>(Instance);
}
