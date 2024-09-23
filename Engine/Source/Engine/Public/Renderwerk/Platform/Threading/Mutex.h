#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include <Windows.h>

struct ENGINE_API FMutex
{
public:
	FMutex();
	~FMutex();

	DEFINE_DEFAULT_COPY_AND_MOVE(FMutex)

public:
	bool TryLock();
	void Lock();
	void Unlock();

public:
	[[nodiscard]] CRITICAL_SECTION GetHandle() const { return CriticalSection; }

private:
	CRITICAL_SECTION CriticalSection;
};

struct ENGINE_API FScopedLock
{
public:
	FScopedLock(FMutex& InMutex);
	~FScopedLock();

	DELETE_COPY_AND_MOVE(FScopedLock)

private:
	FMutex& Mutex;
};
