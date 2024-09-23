#include "pch.h"

#include "Renderwerk/Platform/Threading/Signal.h"

FSignal::FSignal()
{
	InitializeConditionVariable(&ConditionVariable);
}

FSignal::~FSignal() = default;

bool8 FSignal::Wait(const FMutex& Mutex, const uint64 Timeout)
{
	CRITICAL_SECTION CriticalSection = Mutex.GetHandle();
	BOOL Result = SleepConditionVariableCS(&ConditionVariable, &CriticalSection, Timeout);
	return Result != 0;
}

void FSignal::NotifyOne()
{
	WakeConditionVariable(&ConditionVariable);
}

void FSignal::NotifyAll()
{
	WakeAllConditionVariable(&ConditionVariable);
}
