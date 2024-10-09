#pragma once

#include "Renderwerk/Application/Application.h"
#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Jobs/JobSystem.h"
#include "Renderwerk/Memory/DeletionQueue.h"

#include "Renderwerk/Memory/SmartPointers.h"
#include "Renderwerk/Platform/WindowManager.h"
#include "Renderwerk/Renderer/Renderer.h"

class ENGINE_API FEngine
{
public:
	FEngine(const TSharedPtr<IApplication>& Application);
	~FEngine();

public:
	void RequestShutdown();

private:
	void Launch();

	void Initialize();
	void RunLoop();
	void Shutdown();

public:
	[[nodiscard]] TSharedPtr<FJobSystem> GetJobSystem() const { return JobSystem; }
	[[nodiscard]] TSharedPtr<FWindowManager> GetWindowManager() const { return WindowManager; }
	[[nodiscard]] TSharedPtr<FWindow> GetMainWindow() const { return MainWindow; }

	[[nodiscard]] bool IsShutdownRequested() const { return bIsShutdownRequested; }

private:
	TSharedPtr<IApplication> Application;

	bool bIsShutdownRequested = false;

	FDeletionQueue DeletionQueue;

	TSharedPtr<FJobSystem> JobSystem;

	TSharedPtr<FWindowManager> WindowManager;
	TSharedPtr<FWindow> MainWindow;

	TSharedPtr<FRenderer> Renderer;

	friend void Launch(const TSharedPtr<IApplication>& Application);
	friend void Shutdown();
};

ENGINE_API extern TSharedPtr<FEngine> GEngine;

ENGINE_API TSharedPtr<FEngine> GetEngine();
