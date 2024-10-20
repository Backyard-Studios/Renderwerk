#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Engine/Subsystem.h"
#include "Renderwerk/Logging/LogCategory.h"
#include "Renderwerk/Threading/Thread.h"

DECLARE_LOG_CATEGORY(LogJob, Trace);

enum class RENDERWERK_API EJobStatus : uint8
{
	Idle,
	Running,
	Finished,
};

template <typename T>
struct RENDERWERK_API FJobHandle
{
public:
	FJobHandle() = default;

	FJobHandle(TFuture<T> InFuture)
		: Future(std::move(InFuture))
	{
	}

	~FJobHandle() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FJobHandle);

public:
	NODISCARD bool8 IsReady() const
	{
		if (!IsValid())
			return false;
		return Future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}

	NODISCARD T GetResult()
	{
		return Future.get();
	}

	NODISCARD bool8 IsValid() const
	{
		return Future.valid();
	}

private:
	TFuture<T> Future;
};

/**
 * @brief Job subsystem that allows to add jobs to a queue that are executed by worker threads.
 */
class RENDERWERK_API FJobSubsystem : public ISubsystem
{
private:
	using FJobFunction = TFunction<void()>;

	struct RENDERWERK_API FJobWorker
	{
		TSharedPtr<FThread> Thread;
		bool8 bIsIdle = true;
	};

public:
	FJobSubsystem();
	~FJobSubsystem() override;

	DELETE_COPY_AND_MOVE(FJobSubsystem);

public:
	void StopAndWaitForRemainingJobs();

	/**
	 * @brief Adds a job to the queue.
	 * @note It's possible to add new jobs inside of job functions, but you are not able to wait for the result of the job.
	 * @tparam TFunction Type of the function to execute.
	 * @tparam TArguments Type of the arguments to pass to the function.
	 * @tparam TReturnType Type of the return value of the function.
	 * @param Function Function to execute.
	 * @param Arguments Arguments to pass to the function.
	 * @return Handle to the job.
	 */
	template <typename TFunction, typename... TArguments, typename TReturnType = std::invoke_result_t<TFunction, TArguments...>>
	FJobHandle<TReturnType> AddJob(TFunction&& Function, TArguments&&... Arguments)
	{
		RW_PROFILING_MARK_FUNCTION();

		auto Job = MakeShared<std::packaged_task<TReturnType()>>(std::bind(std::forward<TFunction>(Function), std::forward<TArguments>(Arguments)...));
		std::future<TReturnType> Future = Job->get_future();
		{
			FScopedLock Lock(Mutex);
			JobQueue.push([Job] { (*Job)(); });
		}
		Signal.NotifyOne();
		return {std::move(Future)};
	}

private:
	void Initialize() override;
	void Shutdown() override;

	void Worker(uint32 Index);

private:
	TVector<FJobWorker> Workers;

	FMutex Mutex;
	FSignal Signal;
	TAtomic<bool8> bIsShutdownRequested;

	TQueue<FJobFunction> JobQueue;
};
