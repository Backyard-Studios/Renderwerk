#include "pch.h"

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
}

void FEngine::RequestShutdown()
{
	bIsShutdownRequested = true;
}

void FEngine::Initialize()
{
	JobSystem = MakeShared<FJobSystem>(4);
	DQ_ADD(JobSystem);

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

		if (!MainWindow->DidResize())
		{
			Renderer->BeginFrame();
			{
			}
			Renderer->EndFrame();
		}
		else
		{
			Renderer->Resize();
			MainWindow->ResetResizeFlag();
		}

		RW_PROFILING_MARK_FRAME();
	}
}

void FEngine::Shutdown()
{
	if (JobSystem)
		JobSystem->StopAndWaitForRemainingJobs();
	DeletionQueue.Flush();
}

TSharedPtr<FEngine> GetEngine()
{
	RW_DEBUG_ASSERT(GEngine, "Engine is not initialized")
	return GEngine;
}
