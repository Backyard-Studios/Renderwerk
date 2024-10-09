#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include <Windows.h>

/**
 * @brief A mutex is a synchronization primitive that can be used to protect shared data from being simultaneously accessed by multiple threads.
 */
struct ENGINE_API FMutex
{
public:
	FMutex();
	~FMutex();

	DEFINE_DEFAULT_COPY_AND_MOVE(FMutex);

public:
	/**
	 * @brief Attempts to lock the mutex.
	 * @return true if the lock was acquired, false otherwise.
	 */
	[[nodiscard]] bool8 TryLock();

	/**
	 * @brief Locks the mutex.
	 */
	void Lock();

	/**
	 * @brief Unlocks the mutex.
	 */
	void Unlock();

public:
	[[nodiscard]] CRITICAL_SECTION GetHandle() const { return CriticalSection; }

	[[nodiscard]] bool8 IsLocked() const { return bIsLocked; }

private:
	CRITICAL_SECTION CriticalSection;

	bool8 bIsLocked = false;
};

/**
 * @brief A scoped lock is a lock that is automatically acquired when the object is created and released when the object is destroyed.
 */
struct ENGINE_API FScopedLock
{
public:
	FScopedLock(FMutex& InMutex);
	~FScopedLock();

	DELETE_COPY_AND_MOVE(FScopedLock);

public:
	void Lock();
	void Unlock();

private:
	FMutex& Mutex;

	bool bIsLocked = false;
};
