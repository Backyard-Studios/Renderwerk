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

/**
 * @brief A simple thread class that wraps the Windows API thread functions.
 */
class ENGINE_API FThread
{
public:
	using FThreadFunction = TFunction<void()>;

public:
	FThread();
	~FThread();

	DEFINE_DEFAULT_COPY_AND_MOVE(FThread)

public:
	[[nodiscard]] FResult Initialize(const FThreadFunction& InThreadFunction, const EThreadPriority& InPriority = EThreadPriority::Normal);
	void Destroy();

public:
	void Start();
	void Join();
	void ForceKill(bool bWaitForCompletion = false);

public:
	[[nodiscard]] HANDLE GetHandle() const { return ThreadHandle; }

	[[nodiscard]] EThreadPriority GetPriority() const { return Priority; }
	[[nodiscard]] uint64 GetThreadId() const { return ThreadId; }
	[[nodiscard]] EThreadState GetState() const { return State; }

private:
	static int32 ConvertThreadPriority(const EThreadPriority& InPriority);

protected:
	FThreadFunction ThreadFunction;
	EThreadPriority Priority = EThreadPriority::Normal;

	HANDLE ThreadHandle = nullptr;
	uint64 ThreadId = 0;
	EThreadState State = EThreadState::Idle;
};
