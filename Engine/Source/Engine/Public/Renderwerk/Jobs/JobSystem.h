#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Platform/Threading/Thread.h"

template <typename T>
struct ENGINE_API FJobHandle
{
public:
	FJobHandle()
	{
	}

	FJobHandle(std::future<T> InFuture)
		: Future(std::move(InFuture))
	{
	}

	~FJobHandle()
	{
	}

	DEFINE_DEFAULT_COPY_AND_MOVE(FJobHandle);

public:
	bool8 IsReady() const
	{
		return Future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}

	T GetResult()
	{
		return Future.get();
	}

private:
	std::future<T> Future;

	friend class FJobSystem;
};

class ENGINE_API FJobSystem
{
public:
	using FJobFunction = TFunction<void()>;

public:
	FJobSystem(size64 InWorkerCount);
	~FJobSystem();

	DELETE_COPY_AND_MOVE(FJobSystem);

public:
	void StopAndWaitForRemainingJobs();

	template <typename TFunction, typename... TArguments, typename TReturnType = std::invoke_result_t<TFunction, TArguments...>>
	FJobHandle<TReturnType> AddJob(TFunction&& Function, TArguments&&... Arguments)
	{
		auto Job = MakeShared<std::packaged_task<TReturnType()>>(std::bind(std::forward<TFunction>(Function), std::forward<TArguments>(Arguments)...));
		std::future<TReturnType> Future = Job->get_future();
		{
			FScopedLock Lock(JobQueueMutex);
			JobQueue.push([Job]() { (*Job)(); });
		}
		JobSignal.NotifyOne();
		return {std::move(Future)};
	}

private:
	void ThreadWorker();

private:
	size64 WorkerCount;

	TVector<TSharedPtr<FThread>> WorkerThreads;

	FMutex JobQueueMutex;
	TQueue<FJobFunction> JobQueue;
	TAtomic<bool8> bIsShutdownRequested;
	FSignal JobSignal;
};
