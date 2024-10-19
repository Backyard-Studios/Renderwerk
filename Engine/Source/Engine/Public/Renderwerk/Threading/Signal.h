#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Threading/Mutex.h"

/**
 * @brief A signal is a synchronization primitive that allows threads to wait for a notification.
 */
class RENDERWERK_API FSignal
{
public:
	FSignal();
	~FSignal();

	DEFINE_DEFAULT_COPY_AND_MOVE(FSignal);

public:
	/**
	 * Wait for the signal to be notified. This will block the current thread.
	 * @note This uses the internal mutex.
	 * @param Timeout The timeout in milliseconds.
	 */
	NODISCARD bool8 Wait(uint64 Timeout = INFINITE);

	/**
	 * @brief Wait for the signal to be notified. This will block the current thread. If the predicate returns false, the waiting is interrupted.
	 * @note This is using the internal mutex.
	 * @param Predicate The predicate to check before waiting.
	 * @param Timeout The timeout in milliseconds.
	 * @return True if the signal was notified, false if the timeout was reached.
	 */
	NODISCARD bool8 Wait(const std::function<bool8()>& Predicate, uint64 Timeout = INFINITE);

	/**
	 * Wait for the signal to be notified. This will block the current thread.
	 * @param InMutex The mutex to use for waiting.
	 * @param Timeout The timeout in milliseconds.
	 */
	NODISCARD bool8 Wait(FMutex& InMutex, uint64 Timeout = INFINITE);

	/**
	 * @brief Wait for the signal to be notified. This will block the current thread. If the predicate returns false, the waiting is interrupted.
	 * @note This is using the internal mutex.
	 * @param InMutex The mutex to use for waiting.
	 * @param Predicate The predicate to check before waiting.
	 * @param Timeout The timeout in milliseconds.
	 * @return True if the signal was notified, false if the timeout was reached.
	 */
	NODISCARD bool8 Wait(FMutex& InMutex, const std::function<bool8()>& Predicate, uint64 Timeout = INFINITE);

	/**
	 * Notify one waiting thread.
	 */
	void NotifyOne();

	/**
	 * Notify all waiting threads.
	 */
	void NotifyAll();

public:
	NODISCARD CONDITION_VARIABLE GetHandle() const { return ConditionVariable; }

private:
	CONDITION_VARIABLE ConditionVariable;

	FMutex Mutex;
};
