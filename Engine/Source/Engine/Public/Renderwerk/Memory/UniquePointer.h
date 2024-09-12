#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Memory/Memory.h"

template <typename T>
class TUniquePointer
{
public:
	TUniquePointer() = default;

	TUniquePointer(const nullptr_t InNullptr)
		: Pointer(InNullptr)
	{
	}

	TUniquePointer(T* InPointer)
		: Pointer(InPointer)
	{
		printf("TUniquePointer::TUniquePointer(T* InPointer)\n");
	}

	TUniquePointer(const TUniquePointer& Other)
		: Pointer(Other.Pointer)
	{
		Other.Pointer = nullptr;
		printf("TUniquePointer::TUniquePointer(const TUniquePointer& Other)\n");
	}

	TUniquePointer(TUniquePointer&& Other) noexcept
		: Pointer(Other.Pointer)
	{
		Other.Pointer = nullptr;
		printf("TUniquePointer::TUniquePointer(TUniquePointer&& Other)\n");
	}

	template <typename TOther>
	TUniquePointer(const TUniquePointer<TOther>& Other)
		: Pointer(Other.Pointer)
	{
		Other.Pointer = nullptr;
		printf("TUniquePointer::TUniquePointer(const TUniquePointer<TOther>& Other)\n");
	}

	template <typename TOther>
	TUniquePointer(TUniquePointer<TOther>&& Other) noexcept
		: Pointer(Other.Pointer)
	{
		Other.Pointer = nullptr;
		printf("TUniquePointer::TUniquePointer(TUniquePointer<TOther>&& Other)\n");
	}

	~TUniquePointer()
	{
		Reset();
	}

public:
	TUniquePointer& operator=(const TUniquePointer& Other)
	{
		printf("TUniquePointer::operator=(const TUniquePointer& Other)\n");
		if (this == &Other)
			return *this;
		Reset(Other.Pointer);
		Other.Reset();
		return *this;
	}

	TUniquePointer& operator=(TUniquePointer&& Other) noexcept
	{
		printf("TUniquePointer::operator=(TUniquePointer&& Other)\n");
		if (this == &Other)
			return *this;
		Reset(Other.Pointer);
		Other.Reset();
		return *this;
	}

	template <typename TOther>
	TUniquePointer& operator=(const TUniquePointer<TOther>& Other)
	{
		printf("TUniquePointer::operator=(const TUniquePointer<TOther>& Other)\n");
		if (this == &Other)
			return *this;
		Reset(Other.Pointer);
		Other.Reset();
		return *this;
	}

	template <typename TOther>
	TUniquePointer& operator=(TUniquePointer<TOther>&& Other) noexcept
	{
		printf("TUniquePointer::operator=(TUniquePointer<TOther>&& Other)\n");
		if (this == &Other)
			return *this;
		Reset(Other.Pointer);
		Other.Reset();
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
	void Reset(T* NewInstance = nullptr)
	{
		if (Pointer == NewInstance)
			return;
		delete Pointer;
		Pointer = NewInstance;
	}

	[[nodiscard]] bool IsValid() const { return Pointer != nullptr; }

public:
	[[nodiscard]] T* GetRaw() { return Pointer; }
	[[nodiscard]] const T* GetRaw() const { return Pointer; }

private:
	mutable T* Pointer = nullptr;
};

template <typename T, typename... TArguments>
[[nodiscard]] TUniquePointer<T> MakeUnique(TArguments&&... Arguments)
{
	return TUniquePointer<T>(new T(std::forward<TArguments>(Arguments)...));
}
