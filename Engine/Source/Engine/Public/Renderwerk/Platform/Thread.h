#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"

enum class ENGINE_API EThreadPriority : uint8
{
	Low,
	Normal,
	High,
};

enum class ENGINE_API EThreadState : uint8
{
	Idle,
	Running,
	Finished
};

class ENGINE_API IThread
{
public:
	using FThreadFunction = TFunction<void()>;

public:
	IThread(FThreadFunction InThreadFunction, const EThreadPriority& InPriority = EThreadPriority::Normal)
		: ThreadFunction(std::move(InThreadFunction)), Priority(InPriority)
	{
	}

	virtual ~IThread() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(IThread)

public:
	virtual void Start() = 0;
	virtual void Join() = 0;
	virtual void ForceKill(bool bWaitForCompletion = false) = 0;

public:
	[[nodiscard]] virtual void* GetNativeHandle() const = 0;

public:
	[[nodiscard]] EThreadPriority GetPriority() const { return Priority; }
	[[nodiscard]] uint64 GetThreadId() const { return ThreadId; }
	[[nodiscard]] EThreadState GetState() const { return State; }

protected:
	FThreadFunction ThreadFunction;
	EThreadPriority Priority;

	uint64 ThreadId = 0;
	EThreadState State = EThreadState::Idle;
};
