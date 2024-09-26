#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Memory/Memory.h"

template <typename T, typename... TArguments>
class ENGINE_API TFrameBuffer
{
public:
	TFrameBuffer(const size64 InSize, TArguments&&... Arguments)
		: Size(InSize), Buffer(FMemory::NewArray<T>(Size, std::forward<TArguments>(Arguments)...)), Index(0)
	{
	}

	~TFrameBuffer()
	{
		FMemory::DeleteArray(Buffer, Size);
	}

	TFrameBuffer(const TFrameBuffer& Other)
		: Size(Other.Size), Buffer(Other.Buffer), Index(Other.Index)
	{
	}

	TFrameBuffer(TFrameBuffer&& Other) noexcept
		: Size(Other.Size), Buffer(Other.Buffer), Index(Other.Index)
	{
	}

	TFrameBuffer& operator=(const TFrameBuffer& Other)
	{
		if (this == &Other)
			return *this;
		Size = Other.Size;
		Buffer = Other.Buffer;
		Index = Other.Index;
		return *this;
	}

	TFrameBuffer& operator=(TFrameBuffer&& Other) noexcept
	{
		if (this == &Other)
			return *this;
		Size = Other.Size;
		Buffer = Other.Buffer;
		Index = Other.Index;
		return *this;
	}

public:
	[[nodiscard]] size64 GetSize() const { return Size; }

	[[nodiscard]] T* GetBuffer(const size64 InIndex) const
	{
		FScopedLock Lock(Mutex);
		return Buffer + (sizeof(T) * InIndex);
	}

	[[nodiscard]] T* GetCurrentBuffer() const
	{
		FScopedLock Lock(Mutex);
		return GetBuffer(GetCurrentIndex());
	}

	[[nodiscard]] size64 GetCurrentIndex() const { return Index; }

	void AdvanceIndex()
	{
		FScopedLock Lock(Mutex);
		Index = (Index + 1) % Size;
	}

private:
	size64 Size;
	T* Buffer;

	mutable FMutex Mutex;
	size64 Index;
};
