﻿#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

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
	DQ_ADD_CUSTOM(WindowManager, WindowManager->ClearRemoveQueue());

	DQ_ADD_CUSTOM(Application, Application->Shutdown());
	Application->Initialize();
	RW_LOG_INFO("Application \"{}\" v{} initialized", Application->GetMetadata().Name, FormatVersion(Application->GetMetadata().Version));

	FWindowSettings WindowSettings = {};
	MainWindow = WindowManager->Create(WindowSettings);
	DQ_ADD_CUSTOM_PREDICATE(MainWindow, WindowManager, WindowManager->Remove(MainWindow));
	MainWindow->SetTitle(Application->GetMetadata().Name + " v" + FormatVersion(Application->GetMetadata().Version));
	MainWindow->Show();

	FRendererSettings RendererSettings = {};
	Renderer = MakeShared<FRenderer>(RendererSettings);
	DQ_ADD(Renderer);

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

		Renderer->BeginFrame();
		{
		}
		Renderer->EndFrame();

		RW_PROFILING_MARK_FRAME();
	}
}

void FEngine::Shutdown()
{
	DeletionQueue.Flush();
}

TSharedPtr<FEngine> GetEngine()
{
	RW_DEBUG_ASSERT(GEngine, "Engine is not initialized")
	return GEngine;
}
