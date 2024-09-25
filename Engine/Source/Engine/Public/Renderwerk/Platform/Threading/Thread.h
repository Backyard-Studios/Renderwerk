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

struct ENGINE_API FThreadContext
{
	uint64 ThreadId = 0;
	std::string Tag = "UnnamedThread";
	EThreadState State = EThreadState::Idle;
};

#define BIND_THREAD_FUNCTION(Function) std::bind(&Function, this, std::placeholders::_1, std::placeholders::_2)

/**
 * @brief A simple thread class that wraps the Windows API thread functions.
 */
class ENGINE_API FThread
{
public:
	using FThreadFunction = TFunction<void(const FThreadContext& Context, void* UserData)>;

public:
	FThread(FThreadFunction&& InThreadFunction, void* InUserData, const EThreadPriority& InPriority = EThreadPriority::Normal);
	FThread(FThreadFunction&& InThreadFunction, const std::string& InTag, const EThreadPriority& InPriority = EThreadPriority::Normal);
	FThread(FThreadFunction&& InThreadFunction, void* InUserData = nullptr, const std::string& InTag = "UnnamedThread",
	        const EThreadPriority& InPriority = EThreadPriority::Normal);
	~FThread();

	DELETE_COPY_AND_MOVE(FThread)

public:
	void Join();
	void ForceKill(bool bWaitForCompletion = false);

public:
	[[nodiscard]] HANDLE GetHandle() const { return ThreadHandle; }

	[[nodiscard]] std::string GetTag() const { return Context.Tag; }
	[[nodiscard]] EThreadPriority GetPriority() const { return Priority; }
	[[nodiscard]] uint64 GetThreadId() const { return Context.ThreadId; }
	[[nodiscard]] EThreadState GetState() const { return Context.State; }

private:
	static int32 ConvertThreadPriority(const EThreadPriority& InPriority);

protected:
	FThreadFunction ThreadFunction;
	void* UserData = nullptr;
	EThreadPriority Priority = EThreadPriority::Normal;

	HANDLE ThreadHandle = nullptr;
	FThreadContext Context = {};
};
