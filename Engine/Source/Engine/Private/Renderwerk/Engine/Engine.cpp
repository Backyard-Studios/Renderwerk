#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

#include <cassert>

TSharedPointer<FEngine> GEngine = nullptr;

FEngine::FEngine() = default;

FEngine::~FEngine() = default;

void FEngine::Launch()
{
	if (bIsAlreadyLaunched)
		return; // TODO: Log warning
	bIsAlreadyLaunched = true;

	Initialize();
	RunLoop();
	Shutdown();
}

void FEngine::Initialize()
{
}

void FEngine::RunLoop()
{
}

void FEngine::Shutdown()
{
}

TSharedPointer<FEngine> GetEngine()
{
	assert(GEngine);
	return GEngine;
}
