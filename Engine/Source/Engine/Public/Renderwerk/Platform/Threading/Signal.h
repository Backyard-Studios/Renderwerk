#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include "Renderwerk/Platform/Threading/Mutex.h"

#include <Windows.h>

/**
 * @brief A signal is a synchronization primitive that allows threads to wait for a condition to be met.
 */
struct ENGINE_API FSignal
{
public:
	FSignal();
	~FSignal();

	DEFINE_DEFAULT_COPY_AND_MOVE(FSignal)

public:
	/**
	 * @brief Wait for the signal to be notified.
	 * @note This is using the internal mutex.
	 */
	bool8 Wait(uint64 Timeout = INFINITE);

	/**
	 * @brief Wait for the signal to be notified. If the predicate returns false, the waiting is interrupted.
	 * @note This is using the internal mutex.
	 * @param Predicate The predicate to check before waiting.
	 * @param Timeout The timeout in milliseconds.
	 * @return True if the signal was notified, false if the timeout was reached.
	 */
	bool8 Wait(const std::function<bool8()>& Predicate, uint64 Timeout = INFINITE);

	/**
	 * @brief Wait for the signal to be notified, and atomically release the given mutex.
	 * @param InMutex The mutex to release atomically.
	 * @param Timeout The timeout in milliseconds.
	 * @return True if the signal was notified, false if the timeout was reached.
	 */
	bool8 Wait(const FMutex& InMutex, uint64 Timeout = INFINITE);

	/**
	 * @brief Wait for the signal to be notified, and atomically release the given mutex. If the predicate returns false, the waiting is interrupted.
	 * @param InMutex The mutex to release atomically.
	 * @param Predicate The predicate to check before waiting.
	 * @param Timeout The timeout in milliseconds.
	 * @return True if the signal was notified, false if the timeout was reached.
	 */
	bool8 Wait(const FMutex& InMutex, const std::function<bool8()>& Predicate, uint64 Timeout = INFINITE);

	/**
	 * @brief Notify one thread waiting on the signal.
	 */
	void NotifyOne();

	/**
	 * @brief Notify all threads waiting on the signal.
	 */
	void NotifyAll();

public:
	[[nodiscard]] CONDITION_VARIABLE GetHandle() const { return ConditionVariable; }

private:
	CONDITION_VARIABLE ConditionVariable;
	FMutex Mutex;
};
