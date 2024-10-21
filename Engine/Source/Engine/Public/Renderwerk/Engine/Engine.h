#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Engine/SubsystemManager.h"
#include "Renderwerk/Logging/LogCategory.h"

DECLARE_LOG_CATEGORY(LogEngine, Trace);

DECLARE_DELEGATE(SignalReceived, uint32);
DECLARE_MULTICAST_DELEGATE(Tick, float64);

class RENDERWERK_API FEngine
{
public:
	FEngine();
	~FEngine();

	DELETE_COPY_AND_MOVE(FEngine);

public:
	void RequestExit();

public:
	NODISCARD FTickDelegate* GetTickDelegate() { return &OnTick; }
	NODISCARD TSharedPtr<FSubsystemManager> GetSubsystemManager() const { return SubsystemManager; }

private:
	/**
	 * Contains the main loop of the engine.
	 */
	void Run();

	void Initialize();
	void Shutdown();

	void SignalHandler(int Signal);

private:
	static void RegisterInterruptSignals();

private:
	INLINE static FSignalReceivedDelegate OnSignalReceived;

private:
	bool8 bIsRunning = true;

	TSharedPtr<FSubsystemManager> SubsystemManager;
	FTickDelegate OnTick;

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

/**
 * @brief Convenience function to get a subsystem of the specified type.
 * @tparam T The type of the subsystem.
 * @return The subsystem of the specified type.
 */
template <typename T, typename = std::is_base_of<ISubsystem, T>>
RENDERWERK_API INLINE TSharedPtr<T> GetSubsystem()
{
	return GetEngine()->GetSubsystemManager()->Get<T>();
}
