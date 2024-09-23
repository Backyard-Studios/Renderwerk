#include "pch.h"

#include "Renderwerk/Platform/Threading/ConditionVariable.h"

FConditionVariable::FConditionVariable()
{
	InitializeConditionVariable(&ConditionVariable);
}

FConditionVariable::~FConditionVariable() = default;

bool8 FConditionVariable::Wait(const FMutex& Mutex, const uint64 Timeout)
{
	CRITICAL_SECTION CriticalSection = Mutex.GetHandle();
	BOOL Result = SleepConditionVariableCS(&ConditionVariable, &CriticalSection, Timeout);
	return Result != 0;
}

void FConditionVariable::NotifyOne()
{
	WakeConditionVariable(&ConditionVariable);
}

void FConditionVariable::NotifyAll()
{
	WakeAllConditionVariable(&ConditionVariable);
}
