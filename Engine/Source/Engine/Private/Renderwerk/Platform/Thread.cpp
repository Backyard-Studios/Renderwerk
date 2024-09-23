#include "pch.h"

#include "Renderwerk/Platform/Thread.h"

FThread::FThread(const FThreadFunction& InThreadFunction, const EThreadPriority& InPriority)
	: ThreadFunction(InThreadFunction), Priority(InPriority)
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
	if (ThreadHandle)
		CloseHandle(ThreadHandle);
}

void FThread::Start()
{
	DWORD Result = ResumeThread(ThreadHandle);
	RW_ASSERT(Result != -1, RESULT_FAILED, "Failed to resume thread");
	State = EThreadState::Running;
}

void FThread::Join()
{
	DWORD Result = WaitForSingleObject(ThreadHandle, INFINITE);
	RW_ASSERT(Result == WAIT_OBJECT_0, RESULT_FAILED, "Failed to wait for thread");
	State = EThreadState::Finished;
}

void FThread::ForceKill(const bool bWaitForCompletion)
{
	if (bWaitForCompletion)
		Join();

	bool Result = TerminateThread(ThreadHandle, 0);
	RW_ASSERT(Result, RESULT_FAILED, "Failed to terminate thread");
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
