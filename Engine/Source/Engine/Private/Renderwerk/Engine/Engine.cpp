#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

#include <cassert>

TSharedPtr<FEngine> GEngine = nullptr;

FEngine::FEngine(const TSharedPtr<IApplication>& Application)
	: Application(Application)
{
}

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
		WindowManager.reset();
	});

	DeletionQueue.Add([this]()
	{
		if (Application)
			Application->Shutdown();
		Application.reset();
	});
	Application->Initialize();
	RW_LOG_INFO("Application \"{}\" v{} initialized", Application->GetMetadata().Name, FormatVersion(Application->GetMetadata().Version));

	FWindowSettings WindowSettings = {};
	MainWindow = WindowManager->Create(WindowSettings);
	DeletionQueue.Add([this]()
	{
		if (MainWindow && WindowManager)
			WindowManager->Remove(MainWindow);
		MainWindow.reset();
	});
	MainWindow->AppendTitle(" [App: " + Application->GetMetadata().Name + ", AppVersion: v" + FormatVersion(Application->GetMetadata().Version) + "]");

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

TSharedPtr<FEngine> GetEngine()
{
	assert(GEngine);
	return GEngine;
}
