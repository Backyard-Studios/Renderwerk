#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Memory/DeletionQueue.h"

#include "Renderwerk/Memory/SharedPointer.h"
#include "Renderwerk/Platform/WindowManager.h"

class ENGINE_API FEngine
{
public:
	FEngine(const TSharedPointer<IApplication>& Application);
	~FEngine();

public:
	void RequestShutdown();

private:
	void Launch();

	void Initialize();
	void RunLoop();
	void Shutdown();

public:
	[[nodiscard]] TSharedPointer<FWindowManager> GetWindowManager() const { return WindowManager; }
	[[nodiscard]] TSharedPointer<FWindow> GetMainWindow() const { return MainWindow; }

	[[nodiscard]] bool IsShutdownRequested() const { return bIsShutdownRequested; }

private:
	TSharedPointer<IApplication> Application;

	bool bIsShutdownRequested = false;

	FDeletionQueue DeletionQueue;
	TSharedPointer<FWindowManager> WindowManager;
	TSharedPointer<FWindow> MainWindow;

	friend void Launch(const TSharedPointer<IApplication>& Application);
	friend void Shutdown();
};

ENGINE_API extern TSharedPointer<FEngine> GEngine;

ENGINE_API TSharedPointer<FEngine> GetEngine();
