#include "pch.h"

#include "Renderwerk/Threading/Thread.h"

int32 ConvertThreadPriority(const EThreadPriority& InPriority)
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

FThread::FThread(FThreadFunction&& InThreadFunction, void* InUserData, const EThreadPriority& InPriority)
	: FThread(std::move(InThreadFunction), InUserData, TEXT("UnnamedThread"), InPriority)
{
}

FThread::FThread(FThreadFunction&& InThreadFunction, const FString& InTag, const EThreadPriority& InPriority)
	: FThread(std::move(InThreadFunction), nullptr, InTag, InPriority)
{
}

FThread::FThread(FThreadFunction&& InThreadFunction, void* InUserData, const FString& InTag, const EThreadPriority& InPriority)
	: Priority(InPriority), ThreadFunction(std::move(InThreadFunction)), UserData(InUserData)
{
	Context = {};
	Context.Tag = InTag;
	Context.State = EThreadState::Idle;

	LPTHREAD_START_ROUTINE Win32ThreadFunction = [](const LPVOID Param) -> DWORD
	{
		FThread* Thread = static_cast<FThread*>(Param);
		Thread->Context.State = EThreadState::Running;

		FString ThreadName = Thread->Context.Tag;
		if (ThreadName == TEXT("UnnamedThread"))
#ifdef RW_USE_ANSI_STRINGS
			ThreadName += std::to_string(Thread->GetThreadId());
#else
			ThreadName += std::to_wstring(Thread->Context.ThreadId);
#endif

		Thread->ThreadFunction(Thread->Context, Thread->UserData);
		Thread->Context.State = EThreadState::Finished;
		return 0;
	};
	ThreadHandle = CreateThread(nullptr, 0, Win32ThreadFunction, this, 0, reinterpret_cast<LPDWORD>(&Context.ThreadId));
	SetThreadPriority(ThreadHandle, ConvertThreadPriority(Priority));
}

FThread::~FThread()
{
	if (Context.State == EThreadState::Running)
		ForceKill();
	if (ThreadHandle != INVALID_HANDLE_VALUE)
		CloseHandle(ThreadHandle);
}

void FThread::Join()
{
	if (Context.State != EThreadState::Running)
		return;
	DWORD WaitResult = WaitForSingleObject(ThreadHandle, 100000);
	ASSERTM(WaitResult == WAIT_OBJECT_0, "Failed to wait for thread")
	BOOL CloseResult = CloseHandle(ThreadHandle);
	ASSERTM(CloseResult != 0, "Failed to close thread handle")
	ThreadHandle = INVALID_HANDLE_VALUE;
	Context.State = EThreadState::Finished;
}

void FThread::ForceKill(const bool8 bWaitForCompletion)
{
	if (Context.State != EThreadState::Running)
		return;
	if (bWaitForCompletion)
		Join();
	bool TerminateResult = TerminateThread(ThreadHandle, GetExitCodeThread(ThreadHandle, nullptr));
	ASSERTM(TerminateResult, "Failed to terminate thread")
	BOOL CloseResult = CloseHandle(ThreadHandle);
	ASSERTM(CloseResult != 0, "Failed to close thread handle")
	ThreadHandle = INVALID_HANDLE_VALUE;
	Context.State = EThreadState::Finished;
}

bool8 FThread::IsValid() const
{
	return ThreadHandle != nullptr;
}
