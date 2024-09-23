#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/Result.h"
#include "Renderwerk/Memory/DeletionQueue.h"

#include "Renderwerk/Memory/SharedPointer.h"
#include "Renderwerk/Platform/WindowManager.h"

class ENGINE_API FEngine
{
public:
	FEngine();
	~FEngine();

public:
	FResult Launch();

	void RequestShutdown();

private:
	FResult Initialize();
	FResult RunLoop();
	void Shutdown();

public:
	[[nodiscard]] TSharedPointer<FWindowManager> GetWindowManager() const { return WindowManager; }
	[[nodiscard]] TSharedPointer<FWindow> GetMainWindow() const { return MainWindow; }

	[[nodiscard]] bool IsShutdownRequested() const { return bIsShutdownRequested; }

private:
	bool bIsAlreadyLaunched = false;
	bool bIsAlreadyShutdown = false;

	bool bIsShutdownRequested = false;

	FDeletionQueue DeletionQueue;
	TSharedPointer<FWindowManager> WindowManager;
	TSharedPointer<FWindow> MainWindow;

	friend void Shutdown();
};

ENGINE_API extern TSharedPointer<FEngine> GEngine;

ENGINE_API TSharedPointer<FEngine> GetEngine();
