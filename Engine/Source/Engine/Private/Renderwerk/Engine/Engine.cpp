#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

#include <cassert>

TSharedPointer<FEngine> GEngine = nullptr;

FEngine::FEngine() = default;

FEngine::~FEngine() = default;

FResult FEngine::Launch()
{
	if (bIsAlreadyLaunched)
		return RESULT_FAILED;
	bIsAlreadyLaunched = true;

	RW_PROFILING_MARK_THREAD("MainThread");

	FResult Result = Initialize();
	if (Result.IsError())
	{
		RW_LOG_ERROR("Failed to initialize engine: {}", Result.GetReason());
		return Result;
	}
	Result = RunLoop();
	if (Result.IsError())
	{
		RW_LOG_ERROR("Failure during loop: {}", Result.GetReason());
		return Result;
	}
	Shutdown();
	return RESULT_SUCCESS;
}

void FEngine::RequestShutdown()
{
	bIsShutdownRequested = true;
}

FResult FEngine::Initialize()
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

	Renderer = MakeShared<FRenderer>();
	DeletionQueue.Add([this]()
	{
		if (Renderer)
			Renderer->Destroy();
		Renderer.Reset();
	});
	FResult RendererInitializeResult = Renderer->Initialize();
	CHECK_RESULT(RendererInitializeResult)

	RW_LOG_INFO("Engine initialized");
	return RESULT_SUCCESS;
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
	return RESULT_SUCCESS;
}

void FEngine::Shutdown()
{
	if (bIsAlreadyShutdown)
		return;
	bIsAlreadyShutdown = true;
	DeletionQueue.Flush();
}

TSharedPointer<FEngine> GetEngine()
{
	assert(GEngine);
	return GEngine;
}
