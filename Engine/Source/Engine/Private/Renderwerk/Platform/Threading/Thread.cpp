#include "pch.h"

#include "Renderwerk/Platform/Threading/Thread.h"

FThread::FThread(FThreadFunction&& InThreadFunction, void* InUserData, const EThreadPriority& InPriority)
	: FThread(std::move(InThreadFunction), InUserData, "UnnamedThread", InPriority)
{
}

FThread::FThread(FThreadFunction&& InThreadFunction, const std::string& InTag, const EThreadPriority& InPriority)
	: FThread(std::move(InThreadFunction), nullptr, InTag, InPriority)
{
}

FThread::FThread(FThreadFunction&& InThreadFunction, void* InUserData, const std::string& InTag, const EThreadPriority& InPriority)
	: ThreadFunction(std::move(InThreadFunction)), UserData(InUserData), Priority(InPriority)
{
	Context.Tag = InTag;
	Context.State = EThreadState::Running;

	LPTHREAD_START_ROUTINE Win32ThreadFunction = [](const LPVOID Param) -> DWORD
	{
		FThread* Thread = static_cast<FThread*>(Param);

		std::string ThreadName = Thread->Context.Tag;
		if (ThreadName == "UnnamedThread")
			ThreadName += std::to_string(Thread->GetThreadId());
		RW_PROFILING_MARK_THREAD(ThreadName.c_str());

		Thread->ThreadFunction(Thread->Context, Thread->UserData);
		return 0;
	};
	ThreadHandle = CreateThread(nullptr, 0, Win32ThreadFunction, this, 0, reinterpret_cast<LPDWORD>(&Context.ThreadId));
	SetThreadPriority(ThreadHandle, ConvertThreadPriority(Priority));
}

FThread::~FThread()
{
	if (Context.State == EThreadState::Running)
		ForceKill();
	if (ThreadHandle)
		CloseHandle(ThreadHandle);
}

void FThread::Join()
{
	DWORD Result = WaitForSingleObject(ThreadHandle, INFINITE);
	RW_DEBUG_ASSERT(Result == WAIT_OBJECT_0, "Failed to wait for thread")
	Context.State = EThreadState::Finished;
}

void FThread::ForceKill(const bool bWaitForCompletion)
{
	if (bWaitForCompletion)
		Join();

	bool Result = TerminateThread(ThreadHandle, 0);
	RW_DEBUG_ASSERT(Result, "Failed to terminate thread")
	Context.State = EThreadState::Finished;
}

int32 FThread::ConvertThreadPriority(const EThreadPriority& InPriority)
{
	switch (InPriority)
	{
	case EThreadPriority::Low:
		return THREAD_PRIORITY_LOWEST;
	case EThreadPriority::High:
		return THREAD_PRIORITY_HIGHEST;
	case EThreadPriority::Normal:
	default:
		return THREAD_PRIORITY_NORMAL;
	}
}
