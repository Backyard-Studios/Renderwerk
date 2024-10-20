#pragma once
// ReSharper disable CppUnusedIncludeDirective

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/DataTypes/String.h"
#include "Renderwerk/Logging/LogCategory.h"
#include "Renderwerk/Platform/Assert.h"

#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <wrl.h>
#include <d3dx12/d3dx12.h>

using Microsoft::WRL::ComPtr;

#define CHECK_RESULT(ResultPredicate) \
	{ \
		HRESULT UNIQUE_NAME(Result) = ResultPredicate; \
		ASSERTR(SUCCEEDED(UNIQUE_NAME(Result)), UNIQUE_NAME(Result)) \
	}

#define CHECK_RESULTM(ResultPredicate, ...) \
	{ \
		HRESULT UNIQUE_NAME(Result) = ResultPredicate; \
		ASSERTRM(SUCCEEDED(UNIQUE_NAME(Result)), UNIQUE_NAME(Result), __VA_ARGS__) \
	}

#define DEBUG_CHECK_RESULT(ResultPredicate) \
	{ \
		HRESULT UNIQUE_NAME(Result) = ResultPredicate; \
		DEBUG_ASSERTR(SUCCEEDED(UNIQUE_NAME(Result)), UNIQUE_NAME(Result)) \
	}

#define DEBUG_CHECK_RESULTM(ResultPredicate, ...) \
	{ \
		HRESULT UNIQUE_NAME(Result) = ResultPredicate; \
		DEBUG_ASSERTRM(SUCCEEDED(UNIQUE_NAME(Result)), UNIQUE_NAME(Result), __VA_ARGS__) \
	}

DECLARE_LOG_CATEGORY(LogGraphics, Trace);

RENDERWERK_API FString D3D12ResultToString(HRESULT Result);
