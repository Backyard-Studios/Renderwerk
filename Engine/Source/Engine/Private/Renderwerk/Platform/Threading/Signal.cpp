#include "pch.h"

#include "Renderwerk/Platform/Threading/Signal.h"

FSignal::FSignal()
{
	InitializeConditionVariable(&ConditionVariable);
}

FSignal::~FSignal() = default;

bool8 FSignal::Wait(const uint64 Timeout)
{
	return Wait(Mutex, Timeout);
}

bool8 FSignal::Wait(const std::function<bool8()>& Predicate, const uint64 Timeout)
{
	return Wait(Mutex, Predicate, Timeout);
}

bool8 FSignal::Wait(const FMutex& InMutex, const uint64 Timeout)
{
	CRITICAL_SECTION CriticalSection = InMutex.GetHandle();
	BOOL Result = SleepConditionVariableCS(&ConditionVariable, &CriticalSection, Timeout);
	return Result != 0;
}

bool8 FSignal::Wait(const FMutex& InMutex, const std::function<bool8()>& Predicate, const uint64 Timeout)
{
	CRITICAL_SECTION CriticalSection = InMutex.GetHandle();
	while (!Predicate())
	{
		BOOL Result = SleepConditionVariableCS(&ConditionVariable, &CriticalSection, Timeout);
		if (Result == 0)
			return false;
	}
	return true;
}

void FSignal::NotifyOne()
{
	WakeConditionVariable(&ConditionVariable);
}

void FSignal::NotifyAll()
{
	WakeAllConditionVariable(&ConditionVariable);
}
