#include "pch.h"

#include "Renderwerk/Jobs/JobSubsystem.h"

#define JOB_SYSTEM_MIN_THREADS FORWARD(2)

DEFINE_LOG_CATEGORY(LogJob);

FJobSubsystem::FJobSubsystem() = default;

FJobSubsystem::~FJobSubsystem() = default;

void FJobSubsystem::StopAndWaitForRemainingJobs()
{
	for (size64 Index = 0; Index < JobQueue.size(); ++Index)
		Signal.NotifyOne();
	bIsShutdownRequested = true;
	Signal.NotifyAll();
	for (FJobWorker& JobWorker : Workers)
	{
		JobWorker.Thread->Join();
		JobWorker.Thread.reset();
	}
	Workers.clear();
}

void FJobSubsystem::Initialize()
{
	FLogManager::RegisterLogCategory(LogJob);

	FProcessorInfo ProcessorInfo = GetPlatform()->GetProcessorInfo();
	uint32 ThreadCount = std::max(static_cast<uint32>(JOB_SYSTEM_MIN_THREADS), ProcessorInfo.LogicalCoreCount / 2);
	for (uint32 Index = 0; Index < ThreadCount; ++Index)
	{
		Workers.push_back({
			.Thread = MakeShared<FThread>(std::move([=](const FThreadContext& Context, void* UserData)
			{
				Worker(Index);
			})),
			.bIsIdle = true
		});
	}

	RW_LOG(LogJob, Info, "Initialized job system with {} threads", ThreadCount);
}

void FJobSubsystem::Shutdown()
{
	StopAndWaitForRemainingJobs();
}

void FJobSubsystem::Worker(const uint32 Index)
{
	FJobWorker& JobWorker = Workers.at(Index);
	JobWorker.bIsIdle = true;

	while (!bIsShutdownRequested)
	{
		JobWorker.bIsIdle = true;
		bool8 Result = Signal.Wait([=]() { return bIsShutdownRequested || !JobQueue.empty(); });
		ASSERTM(Result, "Failed to wait for signal");

		FScopedLock Lock(Mutex);
		if (bIsShutdownRequested && JobQueue.empty())
			continue;

		FJobFunction Job = std::move(JobQueue.front());
		JobQueue.pop();
		JobWorker.bIsIdle = false;
		Lock.Unlock();
		Job();
		Lock.Lock();
		JobWorker.bIsIdle = true;
	}
}
