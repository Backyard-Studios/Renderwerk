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

enum class RENDERWERK_API EFeatureLevel
{
	None = D3D_FEATURE_LEVEL_1_0_CORE,
	Level_11_0 = D3D_FEATURE_LEVEL_11_0,
	Level_11_1 = D3D_FEATURE_LEVEL_11_1,
	Level_12_0 = D3D_FEATURE_LEVEL_12_0,
	Level_12_1 = D3D_FEATURE_LEVEL_12_1,
	Level_12_2 = D3D_FEATURE_LEVEL_12_2,
};

RENDERWERK_API FString ToString(const EFeatureLevel& FeatureLevel);

enum class RENDERWERK_API EShaderModel
{
	None = D3D_SHADER_MODEL_NONE,
	SM_5_1 = D3D_SHADER_MODEL_5_1,
	SM_6_0 = D3D_SHADER_MODEL_6_0,
	SM_6_1 = D3D_SHADER_MODEL_6_1,
	SM_6_2 = D3D_SHADER_MODEL_6_2,
	SM_6_3 = D3D_SHADER_MODEL_6_3,
	SM_6_4 = D3D_SHADER_MODEL_6_4,
	SM_6_5 = D3D_SHADER_MODEL_6_5,
	SM_6_6 = D3D_SHADER_MODEL_6_6,
	SM_6_7 = D3D_SHADER_MODEL_6_7,
	SM_6_8 = D3D_SHADER_MODEL_6_8,
	SM_6_9 = D3D_SHADER_MODEL_6_9,
};

RENDERWERK_API FString ToString(const EShaderModel& ShaderModel);

enum class RENDERWERK_API ERayTracingTier
{
	None = D3D12_RAYTRACING_TIER_NOT_SUPPORTED,
	Tier_1_0 = D3D12_RAYTRACING_TIER_1_0,
	Tier_1_1 = D3D12_RAYTRACING_TIER_1_1,
};

RENDERWERK_API FString ToString(const ERayTracingTier& RayTracingTier);

enum class RENDERWERK_API EVariableShadingRateTier
{
	None = D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED,
	Tier_1 = D3D12_VARIABLE_SHADING_RATE_TIER_1,
	Tier_2 = D3D12_VARIABLE_SHADING_RATE_TIER_2,
};

RENDERWERK_API FString ToString(const EVariableShadingRateTier& VariableShadingRateTier);

enum class RENDERWERK_API EMeshShaderTier
{
	None = D3D12_MESH_SHADER_TIER_NOT_SUPPORTED,
	Tier_1 = D3D12_MESH_SHADER_TIER_1,
};

RENDERWERK_API FString ToString(const EMeshShaderTier& MeshShaderTier);
