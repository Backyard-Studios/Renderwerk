#pragma once

#include "Renderwerk/Memory/SharedPointer.h"

template <typename T>
class TWeakPointer
{
public:
	TWeakPointer() = default;

	TWeakPointer(TSharedPointer<T> SharedPointer)
		: Pointer(SharedPointer.GetRaw())
	{
	}

	TWeakPointer(T* InPointer)
		: Pointer(InPointer)
	{
	}

public:
	T* operator->() { return Pointer; }
	const T* operator->() const { return Pointer; }

	T& operator*() { return *Pointer; }
	const T& operator*() const { return *Pointer; }

public:
	template <typename TOther>
	[[nodiscard]] TWeakPointer<TOther> As()
	{
		return TWeakPointer<TOther>(*this);
	}

private:
	T* Pointer;
};
