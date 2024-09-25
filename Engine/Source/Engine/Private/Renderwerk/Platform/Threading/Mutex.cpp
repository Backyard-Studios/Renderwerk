#include "pch.h"

#include "Renderwerk/Platform/Threading/Mutex.h"

FMutex::FMutex()
{
	CriticalSection = {};
	InitializeCriticalSection(&CriticalSection);
}

FMutex::~FMutex()
{
	DeleteCriticalSection(&CriticalSection);
}

bool8 FMutex::TryLock()
{
	BOOL Result = TryEnterCriticalSection(&CriticalSection);
	return Result != 0;
}

void FMutex::Lock()
{
	EnterCriticalSection(&CriticalSection);
	bIsLocked = true;
}

void FMutex::Unlock()
{
	LeaveCriticalSection(&CriticalSection);
	bIsLocked = false;
}

FScopedLock::FScopedLock(FMutex& InMutex)
	: Mutex(InMutex)
{
	Lock();
}

FScopedLock::~FScopedLock()
{
	Unlock();
}

void FScopedLock::Lock()
{
	if (!bIsLocked)
	{
		Mutex.Lock();
		bIsLocked = true;
	}
}

void FScopedLock::Unlock()
{
	if (bIsLocked)
	{
		Mutex.Unlock();
		bIsLocked = false;
	}
}
