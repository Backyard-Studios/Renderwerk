#include "pch.h"

#include "Renderwerk/Jobs/JobSystem.h"

FJobSystem::FJobSystem(const size64 InWorkerCount)
	: WorkerCount(InWorkerCount)
{
	for (size64 Index = 0; Index < WorkerCount; ++Index)
	{
		WorkerThreads.push_back(MakeShared<FThread>([=](const FThreadContext& Context, void* UserData)
		{
			RW_LOG_TRACE("Worker thread {} started", Index);
			ThreadWorker();
			RW_LOG_TRACE("Worker thread {} finished", Index);
		}, std::format("WorkerThread{}", Index)));
	}
}

FJobSystem::~FJobSystem()
{
	if (!WorkerThreads.empty())
		StopAndWaitForRemainingJobs();
}

void FJobSystem::StopAndWaitForRemainingJobs()
{
	for (size64 Index = 0; Index < JobQueue.size(); ++Index)
		JobSignal.NotifyOne();
	bIsShutdownRequested = true;
	JobSignal.NotifyAll();
	uint32 Index = 0;
	for (TSharedPtr<FThread>& WorkerThread : WorkerThreads)
	{
		WorkerThread->Join();
		WorkerThread.reset();
		Index++;
	}
	WorkerThreads.clear();
}

void FJobSystem::ThreadWorker()
{
	while (!bIsShutdownRequested)
	{
		JobSignal.Wait([=]() { return bIsShutdownRequested || !JobQueue.empty(); });
		FScopedLock Lock(JobQueueMutex);

		if (bIsShutdownRequested && JobQueue.empty())
			return;

		FJobFunction Job = std::move(JobQueue.front());
		JobQueue.pop();

		Lock.Unlock();
		Job();
		Lock.Lock();
	}
}
