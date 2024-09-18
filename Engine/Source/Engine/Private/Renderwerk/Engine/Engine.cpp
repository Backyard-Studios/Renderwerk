#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

#include <cassert>

TSharedPointer<FEngine> GEngine = nullptr;

FEngine::FEngine() = default;

FEngine::~FEngine() = default;

FResult FEngine::Launch()
{
	if (bIsAlreadyLaunched)
		return RW_RESULT_CODE_FAIL;
	bIsAlreadyLaunched = true;

	FResult Result = Initialize();
	if (Result.IsError())
		return Result;
	Result = RunLoop();
	if (Result.IsError())
		return Result;
	Shutdown();
	return RW_RESULT_CODE_SUCCESS;
}

FResult FEngine::Initialize()
{
	return RW_RESULT_CODE_SUCCESS;
}

FResult FEngine::RunLoop()
{
	return RW_RESULT_CODE_SUCCESS;
}

void FEngine::Shutdown()
{
	if (bIsAlreadyShutdown)
		return;
	bIsAlreadyShutdown = true;
}

TSharedPointer<FEngine> GetEngine()
{
	assert(GEngine);
	return GEngine;
}
