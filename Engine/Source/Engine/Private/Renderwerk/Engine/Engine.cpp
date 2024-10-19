#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

TSharedPtr<FEngine> GEngine = nullptr;

FEngine::FEngine()
{
}

FEngine::~FEngine()
{
}

TSharedPtr<FEngine> GetEngine()
{
	DEBUG_ASSERTM(GEngine, "Global engine pointer is null");
	return GEngine;
}
