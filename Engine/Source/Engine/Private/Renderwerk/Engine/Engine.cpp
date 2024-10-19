#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

DEFINE_LOG_CATEGORY(LogEngine);

TSharedPtr<FEngine> GEngine = nullptr;

FEngine::FEngine()
{
	RW_LOG(LogEngine, Info, "{} {} by {}", TEXT(RW_ENGINE_NAME), TEXT(RW_ENGINE_FULL_VERSION), TEXT(RW_ENGINE_AUTHOR));
}

FEngine::~FEngine()
{
}

TSharedPtr<FEngine> GetEngine()
{
	DEBUG_ASSERTM(GEngine, "Global engine pointer is null");
	return GEngine;
}
