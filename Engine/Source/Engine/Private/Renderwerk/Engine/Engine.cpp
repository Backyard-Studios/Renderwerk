#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

// Convenience macros for adding objects to the deletion queue
#define DQ_ADD(Object) DeletionQueue.Add([=]() { Object.Reset(); RW_LOG_TRACE("Deleting {}", #Object); })
#define DQ_ADD_CUSTOM(Object, CustomDeletion) DeletionQueue.Add([=]() { if(Object) CustomDeletion; Object.Reset(); RW_LOG_TRACE("Deleting {}", #Object); })
#define DQ_ADD_CUSTOM_PREDICATE(Object, Predicate, CustomDeletion) DeletionQueue.Add([=]() { if(Object && Predicate) CustomDeletion; Object.Reset(); RW_LOG_TRACE("Deleting {}", #Object); })

TSharedPointer<FEngine> GEngine = nullptr;

FEngine::FEngine(const TSharedPointer<IApplication>& Application)
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
	MainWindow->AppendTitle(" [App: " + Application->GetMetadata().Name + ", AppVersion: v" + FormatVersion(Application->GetMetadata().Version) + "]");
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
