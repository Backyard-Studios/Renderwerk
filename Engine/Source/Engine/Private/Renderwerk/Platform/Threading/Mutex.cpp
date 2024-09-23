#include "pch.h"

#include "Renderwerk/Platform/Threading/Mutex.h"

FMutex::FMutex()
{
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
}

void FMutex::Unlock()
{
	LeaveCriticalSection(&CriticalSection);
}

FScopedLock::FScopedLock(FMutex& InMutex)
	: Mutex(InMutex)
{
	Mutex.Lock();
}

FScopedLock::~FScopedLock()
{
	Mutex.Unlock();
}
