#include "pch.h"

#include "Renderwerk/Platform/Win32/Win32Thread.h"

FWin32Thread::FWin32Thread(const FThreadFunction& InThreadFunction, const EThreadPriority& InPriority)
	: IThread(InThreadFunction, InPriority)
{
	LPTHREAD_START_ROUTINE Win32ThreadFunction = [](const LPVOID Param) -> DWORD
	{
		FWin32Thread* Thread = static_cast<FWin32Thread*>(Param);
		Thread->ThreadFunction();
		return 0;
	};
	ThreadHandle = CreateThread(nullptr, 0, Win32ThreadFunction, this, CREATE_SUSPENDED, reinterpret_cast<LPDWORD>(&ThreadId));
	SetThreadPriority(ThreadHandle, ConvertThreadPriority(Priority));
}

FWin32Thread::~FWin32Thread()
{
	if (ThreadHandle)
		CloseHandle(ThreadHandle);
}

void FWin32Thread::Start()
{
	DWORD Result = ResumeThread(ThreadHandle);
	RW_ASSERT(Result != -1, RW_RESULT_CODE_FAIL, "Failed to resume thread");
	State = EThreadState::Running;
}

void FWin32Thread::Join()
{
	DWORD Result = WaitForSingleObject(ThreadHandle, INFINITE);
	RW_ASSERT(Result == WAIT_OBJECT_0, RW_RESULT_CODE_FAIL, "Failed to wait for thread");
	State = EThreadState::Finished;
}

void FWin32Thread::ForceKill(const bool bWaitForCompletion)
{
	if (bWaitForCompletion)
		Join();

	bool Result = TerminateThread(ThreadHandle, 0);
	RW_ASSERT(Result, RW_RESULT_CODE_FAIL, "Failed to terminate thread");
	State = EThreadState::Finished;
}

void* FWin32Thread::GetNativeHandle() const
{
	return ThreadHandle;
}

int32 FWin32Thread::ConvertThreadPriority(const EThreadPriority& InPriority)
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
