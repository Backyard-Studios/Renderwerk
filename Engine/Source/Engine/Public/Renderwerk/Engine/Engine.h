#pragma once

#include "Renderwerk/EngineMinimal.h"

DECLARE_LOG_CATEGORY(LogEngine, Trace);

class RENDERWERK_API FEngine
{
public:
	FEngine();
	~FEngine();

	DELETE_COPY_AND_MOVE(FEngine);

private:
	/**
	 * Contains the main loop of the engine.
	 */
	void Run() const;

	void SignalHandler(int Signal);

private:
	static void RegisterInterruptSignals();

private:
	bool8 bIsRunning = true;

	friend void GuardedMain();
};

/**
 * Global engine pointer. You should use GetEngine() to safely access this pointer.
 * Please check the validity of the pointer before using it.
 */
RENDERWERK_API extern TSharedPtr<FEngine> GEngine;

/**
 * Checks the validity of the global engine pointer and returns it.
 * @return The global engine pointer.
 */
RENDERWERK_API TSharedPtr<FEngine> GetEngine();
