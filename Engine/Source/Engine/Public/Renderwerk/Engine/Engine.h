#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/Result.h"

#include "Renderwerk/Memory/SharedPointer.h"

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

private:
	bool bIsAlreadyLaunched = false;
	bool bIsAlreadyShutdown = false;

	bool bIsShutdownRequested = false;

	friend void Shutdown();
};

ENGINE_API extern TSharedPointer<FEngine> GEngine;

ENGINE_API TSharedPointer<FEngine> GetEngine();
