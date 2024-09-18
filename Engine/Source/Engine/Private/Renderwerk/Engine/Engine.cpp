#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

#include <cassert>

TSharedPointer<FEngine> GEngine = nullptr;

FEngine::FEngine() = default;

FEngine::~FEngine() = default;

FResult FEngine::Launch()
{
	if (bIsAlreadyLaunched)
		return RW_RESULT_CODE_FAIL;
	bIsAlreadyLaunched = true;

	RW_PROFILING_MARK_THREAD("MainThread");

	FResult Result = Initialize();
	if (Result.IsError())
		return Result;
	Result = RunLoop();
	if (Result.IsError())
		return Result;
	Shutdown();
	return RW_RESULT_CODE_SUCCESS;
}

void FEngine::RequestShutdown()
{
	bIsShutdownRequested = true;
}

FResult FEngine::Initialize()
{
	WindowManager = MakeShared<FWindowManager>();
	FWindowSettings WindowSettings = {};
	MainWindow = WindowManager->Create(WindowSettings);
	MainWindow->Show();

	return RW_RESULT_CODE_SUCCESS;
}

FResult FEngine::RunLoop()
{
	while (!bIsShutdownRequested)
	{
		WindowManager->Update();

		if (MainWindow->GetWindowState().bIsClosed)
			RequestShutdown();

		WindowManager->ClearRemoveQueue();

		RW_PROFILING_MARK_FRAME();
	}
	return RW_RESULT_CODE_SUCCESS;
}

void FEngine::Shutdown()
{
	if (bIsAlreadyShutdown)
		return;
	bIsAlreadyShutdown = true;

	if (MainWindow && WindowManager)
		WindowManager->Remove(MainWindow);
	MainWindow.Reset();
	WindowManager.Reset();
}

TSharedPointer<FEngine> GetEngine()
{
	assert(GEngine);
	return GEngine;
}
