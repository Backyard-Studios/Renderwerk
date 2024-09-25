#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

#include <cassert>

TSharedPointer<FEngine> GEngine = nullptr;

FEngine::FEngine() = default;

FEngine::~FEngine() = default;

void FEngine::Launch()
{
	RW_PROFILING_MARK_THREAD("MainThread");

	Initialize();
	RunLoop();
	Shutdown();
}

void FEngine::RequestShutdown()
{
	bIsShutdownRequested = true;
}

void FEngine::Initialize()
{
	WindowManager = MakeShared<FWindowManager>();
	DeletionQueue.Add([this]()
	{
		if (WindowManager)
			WindowManager->ClearRemoveQueue();
		WindowManager.Reset();
	});

	FWindowSettings WindowSettings = {};
	MainWindow = WindowManager->Create(WindowSettings);
	DeletionQueue.Add([this]()
	{
		if (MainWindow && WindowManager)
			WindowManager->Remove(MainWindow);
		MainWindow.Reset();
	});

	MainWindow->Show();

	RW_LOG_INFO("Engine initialized");
}

void FEngine::RunLoop()
{
	while (!bIsShutdownRequested)
	{
		WindowManager->Update();

		if (MainWindow->GetWindowState().bIsClosed)
			RequestShutdown();

		WindowManager->ClearRemoveQueue();

		RW_PROFILING_MARK_FRAME();
	}
}

void FEngine::Shutdown()
{
	DeletionQueue.Flush();
}

TSharedPointer<FEngine> GetEngine()
{
	assert(GEngine);
	return GEngine;
}
