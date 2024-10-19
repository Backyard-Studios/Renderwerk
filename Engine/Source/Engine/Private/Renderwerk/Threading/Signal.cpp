#include "pch.h"

#include "Renderwerk/Threading/Signal.h"

FSignal::FSignal()
{
	ConditionVariable = {};
	InitializeConditionVariable(&ConditionVariable);
	Mutex = FMutex();
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

bool8 FSignal::Wait(FMutex& InMutex, const uint64 Timeout)
{
	if (!InMutex.IsLocked())
		InMutex.Lock();
	CRITICAL_SECTION CriticalSection = InMutex.GetHandle();
	BOOL Result = SleepConditionVariableCS(&ConditionVariable, &CriticalSection, static_cast<DWORD>(Timeout));
	InMutex.Unlock();
	return Result != 0;
}

bool8 FSignal::Wait(FMutex& InMutex, const std::function<bool8()>& Predicate, const uint64 Timeout)
{
	while (!Predicate())
	{
		bool8 Result = Wait(InMutex, Timeout);
		if (!Result)
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
