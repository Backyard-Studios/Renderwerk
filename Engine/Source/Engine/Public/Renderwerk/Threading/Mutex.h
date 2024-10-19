#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

/**
 * @brief A mutex is a synchronization primitive that can be used to protect shared data from being simultaneously accessed by multiple threads.
 */
class RENDERWERK_API FMutex
{
public:
	FMutex();
	~FMutex();

	DEFINE_DEFAULT_COPY_AND_MOVE(FMutex);

public:
	/**
	 * @brief Attempts to lock the mutex.
	 * @return Returns true if the lock was acquired, false otherwise.
	 */
	NODISCARD bool8 TryLock();

	/**
	 * @brief Locks the mutex.
	 */
	void Lock();

	/**
	 * @brief Unlocks the mutex.
	 */
	void Unlock();

public:
	NODISCARD CRITICAL_SECTION GetHandle() const { return CriticalSection; }

	NODISCARD bool8 IsLocked() const { return bIsLocked; }

private:
	CRITICAL_SECTION CriticalSection;

	bool8 bIsLocked = false;
};

/**
 * @brief A scoped lock is a lock that is automatically acquired when the object is created and released when the object is destroyed.
 */
struct RENDERWERK_API FScopedLock
{
public:
	FScopedLock(FMutex& InMutex);
	~FScopedLock();

	DELETE_COPY_AND_MOVE(FScopedLock);

public:
	/**
	 * @brief Locks the mutex manually.
	 */
	void Lock() const;

	/**
	 * @brief Unlocks the mutex manually.
	 */
	void Unlock() const;

private:
	FMutex& Mutex;

	bool bIsLocked = false;
};
