#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

enum class RENDERWERK_API EThreadPriority : uint8
{
	Low,
	Normal,
	High,
};

enum class RENDERWERK_API EThreadState : uint8
{
	Idle,
	Running,
	Finished
};

struct RENDERWERK_API FThreadContext
{
	uint64 ThreadId = 0;
	FString Tag = TEXT("UnnamedThread");
	EThreadState State = EThreadState::Idle;
};

class RENDERWERK_API FThread
{
public:
	using FThreadFunction = TFunction<void(const FThreadContext& Context, void* UserData)>;

public:
	FThread(FThreadFunction&& InThreadFunction, void* InUserData, const EThreadPriority& InPriority = EThreadPriority::Normal);
	FThread(FThreadFunction&& InThreadFunction, const FString& InTag, const EThreadPriority& InPriority = EThreadPriority::Normal);
	FThread(FThreadFunction&& InThreadFunction, void* InUserData = nullptr, const FString& InTag = TEXT("UnnamedThread"),
	        const EThreadPriority& InPriority = EThreadPriority::Normal);
	~FThread();

	DEFINE_DEFAULT_COPY_AND_MOVE(FThread);

public:
	/**
	 * @brief Waits for the thread to finish. This will block the calling thread until the thread has finished.
	 */
	void Join();

	/**
	 * @brief Will forcefully kill the thread. This is not recommended as it can lead to undefined behavior.
	 * @note This should only be used in extreme cases where the thread cannot be stopped gracefully.
	 */
	void ForceKill(bool8 bWaitForCompletion = false);

public:
	NODISCARD HANDLE GetHandle() const { return ThreadHandle; }

	NODISCARD EThreadPriority GetPriority() const { return Priority; }
	NODISCARD uint64 GetThreadId() const { return Context.ThreadId; }
	NODISCARD const FString& GetTag() const { return Context.Tag; }
	NODISCARD EThreadState GetState() const { return Context.State; }

private:
	EThreadPriority Priority;
	FThreadContext Context;
	FThreadFunction ThreadFunction;
	void* UserData;

	HANDLE ThreadHandle = nullptr;
};
