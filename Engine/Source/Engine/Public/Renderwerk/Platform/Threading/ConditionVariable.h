#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include <Windows.h>

struct ENGINE_API FConditionVariable
{
public:
	FConditionVariable();
	~FConditionVariable();

	DEFINE_DEFAULT_COPY_AND_MOVE(FConditionVariable)

public:
	bool8 Wait(const FMutex& Mutex, uint64 Timeout = INFINITE);
	void NotifyOne();
	void NotifyAll();

public:
	[[nodiscard]] CONDITION_VARIABLE GetHandle() const { return ConditionVariable; }

private:
	CONDITION_VARIABLE ConditionVariable;
};
