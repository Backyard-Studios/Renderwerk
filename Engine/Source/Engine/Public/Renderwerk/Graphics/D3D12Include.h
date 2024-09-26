#pragma once

#include <Renderwerk/Core/Assertion.h>

#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

#include <wrl.h>

using Microsoft::WRL::ComPtr;

#define CHECK_RESULT(Result, ...) RW_DEBUG_ASSERT(SUCCEEDED(Result), __VA_ARGS__)
#define CHECK_RESULT_CRITICAL(Result, ...) RW_ASSERT_CRITICAL(SUCCEEDED(Result), "Direct3D 12 Error: %s", __VA_ARGS__)

ENGINE_API INLINE std::string ToString(const D3D_FEATURE_LEVEL FeatureLevel)
{
	switch (FeatureLevel)
	{
	case D3D_FEATURE_LEVEL_1_0_GENERIC: return "D3D_FEATURE_LEVEL_1_0_GENERIC";
	case D3D_FEATURE_LEVEL_1_0_CORE: return "D3D_FEATURE_LEVEL_1_0_CORE";
	case D3D_FEATURE_LEVEL_9_1: return "D3D_FEATURE_LEVEL_9_1";
	case D3D_FEATURE_LEVEL_9_2: return "D3D_FEATURE_LEVEL_9_2";
	case D3D_FEATURE_LEVEL_9_3: return "D3D_FEATURE_LEVEL_9_3";
	case D3D_FEATURE_LEVEL_10_0: return "D3D_FEATURE_LEVEL_10_0";
	case D3D_FEATURE_LEVEL_10_1: return "D3D_FEATURE_LEVEL_10_1";
	case D3D_FEATURE_LEVEL_11_0: return "D3D_FEATURE_LEVEL_11_0";
	case D3D_FEATURE_LEVEL_11_1: return "D3D_FEATURE_LEVEL_11_1";
	case D3D_FEATURE_LEVEL_12_0: return "D3D_FEATURE_LEVEL_12_0";
	case D3D_FEATURE_LEVEL_12_1: return "D3D_FEATURE_LEVEL_12_1";
	case D3D_FEATURE_LEVEL_12_2: return "D3D_FEATURE_LEVEL_12_2";
	default:
		return "Unknown";
	}
}

ENGINE_API INLINE std::string ToString(const D3D_SHADER_MODEL ShaderModel)
{
	switch (ShaderModel)
	{
	case D3D_SHADER_MODEL_5_1: return "D3D_SHADER_MODEL_5_1";
	case D3D_SHADER_MODEL_6_0: return "D3D_SHADER_MODEL_6_0";
	case D3D_SHADER_MODEL_6_1: return "D3D_SHADER_MODEL_6_1";
	case D3D_SHADER_MODEL_6_2: return "D3D_SHADER_MODEL_6_2";
	case D3D_SHADER_MODEL_6_3: return "D3D_SHADER_MODEL_6_3";
	case D3D_SHADER_MODEL_6_4: return "D3D_SHADER_MODEL_6_4";
	case D3D_SHADER_MODEL_6_5: return "D3D_SHADER_MODEL_6_5";
	case D3D_SHADER_MODEL_6_6: return "D3D_SHADER_MODEL_6_6";
	case D3D_SHADER_MODEL_6_7: return "D3D_SHADER_MODEL_6_7";
	case D3D_SHADER_MODEL_6_8: return "D3D_SHADER_MODEL_6_8";
	case D3D_SHADER_MODEL_6_9: return "D3D_SHADER_MODEL_6_9";
	case D3D_SHADER_MODEL_NONE:
	default:
		return "D3D_SHADER_MODEL_NONE";
	}
}
