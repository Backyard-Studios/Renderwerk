﻿#include "pch.h"

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

	FRendererSettings RendererSettings = {};
	RendererSettings.Window = MainWindow;
#ifdef RW_CONFIG_DEBUG
	RendererSettings.bEnableValidation = true;
	RendererSettings.bEnableDebugging = true;
#endif
	Renderer = MakeShared<FRenderer>(RendererSettings);
	RW_CHECK_RESULT(Renderer->Initialize())

	RW_LOG_INFO("Engine initialized");
	return RW_RESULT_CODE_SUCCESS;
}

FResult FEngine::RunLoop()
{
	while (!bIsShutdownRequested)
	{
		WindowManager->Update();

		if (MainWindow->GetWindowState().bIsClosed)
			RequestShutdown();


		RW_CHECK_RESULT(Renderer->BeginFrame())
		{
		}
		RW_CHECK_RESULT(Renderer->EndFrame())

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

	if (Renderer)
		Renderer->Shutdown();
	Renderer.Reset();

	if (MainWindow && WindowManager)
		WindowManager->Remove(MainWindow);
	WindowManager->ClearRemoveQueue();
	MainWindow.Reset();
	WindowManager.Reset();
}

TSharedPointer<FEngine> GetEngine()
{
	assert(GEngine);
	return GEngine;
}
