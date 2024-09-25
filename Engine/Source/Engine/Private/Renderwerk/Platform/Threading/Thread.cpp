#include "pch.h"

#include "Renderwerk/Platform/Threading/Thread.h"

FThread::FThread(FThreadFunction&& InThreadFunction, const EThreadPriority& InPriority)
	: ThreadFunction(std::move(InThreadFunction)), Priority(InPriority)
{
	LPTHREAD_START_ROUTINE Win32ThreadFunction = [](const LPVOID Param) -> DWORD
	{
		FThread* Thread = static_cast<FThread*>(Param);
		Thread->ThreadFunction();
		return 0;
	};
	ThreadHandle = CreateThread(nullptr, 0, Win32ThreadFunction, this, CREATE_SUSPENDED, reinterpret_cast<LPDWORD>(&ThreadId));
	SetThreadPriority(ThreadHandle, ConvertThreadPriority(Priority));
}

FThread::~FThread()
{
	if (State == EThreadState::Running)
		ForceKill();
	if (ThreadHandle)
		CloseHandle(ThreadHandle);
}

void FThread::Start()
{
	DWORD Result = ResumeThread(ThreadHandle);
	RW_DEBUG_ASSERT(Result != -1, "Failed to resume thread")
	State = EThreadState::Running;
}

void FThread::Join()
{
	DWORD Result = WaitForSingleObject(ThreadHandle, INFINITE);
	RW_DEBUG_ASSERT(Result == WAIT_OBJECT_0, "Failed to wait for thread")
	State = EThreadState::Finished;
}

void FThread::ForceKill(const bool bWaitForCompletion)
{
	if (bWaitForCompletion)
		Join();

	bool Result = TerminateThread(ThreadHandle, 0);
	RW_DEBUG_ASSERT(Result, "Failed to terminate thread")
	State = EThreadState::Finished;
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
