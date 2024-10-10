#include "pch.h"

#include "Renderwerk/Jobs/JobSystem.h"

#include "imgui.h"

FJobSystem::FJobSystem(const size64 InWorkerCount)
	: WorkerCount(InWorkerCount)
{
	for (size64 Index = 0; Index < WorkerCount; ++Index)
	{
		WorkerThreads.push_back(MakeShared<FThread>([=](const FThreadContext& Context, void* UserData)
		{
			RW_LOG_TRACE("Worker thread {} started", Index);
			ThreadWorker(Index);
			RW_LOG_TRACE("Worker thread {} finished", Index);
		}, std::format("WorkerThread{}", Index)));

		FScopedLock Lock(StatesMutex);
		WorkerThreadStates.push_back(false);
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

void FJobSystem::DebugUI()
{
	ImGui::Begin("Job System");
	{
		ImGui::Text("Worker Count: %llu", WorkerCount);
		{
			FScopedLock Lock(JobQueueMutex);
			ImGui::Text("Job Queue Size: %llu", JobQueue.size());
		}
		ImGui::Spacing();
		size64 Index = 0;
		for (TSharedPtr<FThread> WorkerThread : WorkerThreads)
		{
			ImGui::Spacing();
			ImGui::Text("%s", WorkerThread->GetTag().c_str());
			{
				FScopedLock Lock(StatesMutex);
				ImGui::Text("\tState: %s", WorkerThreadStates[Index] ? "Working" : "Idle");
			}
			Index++;
		}
	}
	ImGui::End();
}

void FJobSystem::ThreadWorker(const size64 Index)
{
	while (!bIsShutdownRequested)
	{
		{
			FScopedLock StatesLock(StatesMutex);
			WorkerThreadStates[Index] = false;
		}
		JobSignal.Wait([=]() { return bIsShutdownRequested || !JobQueue.empty(); });

		RW_PROFILING_MARK_FUNCTION();

		FScopedLock Lock(JobQueueMutex);

		if (bIsShutdownRequested && JobQueue.empty())
			return;

		FJobFunction Job = std::move(JobQueue.front());
		JobQueue.pop();

		{
			FScopedLock StatesLock(StatesMutex);
			WorkerThreadStates[Index] = true;
		}

		Lock.Unlock();
		Job();
		Lock.Lock();
	}
}
