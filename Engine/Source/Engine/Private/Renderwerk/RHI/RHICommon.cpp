#include "pch.h"

#include "Renderwerk/RHI/RHICommon.h"

#pragma region Utility

DEFINE_LOG_CATEGORY(LogRHI);

#pragma endregion

#pragma region ToString Functions

FString D3D12ResultToString(const HRESULT Result)
{
	switch (Result)
	{
	case D3D12_ERROR_ADAPTER_NOT_FOUND: return TEXT("D3D12_ERROR_ADAPTER_NOT_FOUND");
	case D3D12_ERROR_DRIVER_VERSION_MISMATCH: return TEXT("D3D12_ERROR_DRIVER_VERSION_MISMATCH");
	case DXGI_ERROR_ACCESS_DENIED: return TEXT("DXGI_ERROR_ACCESS_DENIED");
	case DXGI_ERROR_ACCESS_LOST: return TEXT("DXGI_ERROR_ACCESS_LOST");
	case DXGI_ERROR_ALREADY_EXISTS: return TEXT("DXGI_ERROR_ALREADY_EXISTS");
	case DXGI_ERROR_CANNOT_PROTECT_CONTENT: return TEXT("DXGI_ERROR_CANNOT_PROTECT_CONTENT");
	case DXGI_ERROR_DEVICE_HUNG: return TEXT("DXGI_ERROR_DEVICE_HUNG");
	case DXGI_ERROR_DEVICE_REMOVED: return TEXT("DXGI_ERROR_DEVICE_REMOVED");
	case DXGI_ERROR_DEVICE_RESET: return TEXT("DXGI_ERROR_DEVICE_RESET");
	case DXGI_ERROR_DRIVER_INTERNAL_ERROR: return TEXT("DXGI_ERROR_DRIVER_INTERNAL_ERROR");
	case DXGI_ERROR_FRAME_STATISTICS_DISJOINT: return TEXT("DXGI_ERROR_FRAME_STATISTICS_DISJOINT");
	case DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE: return TEXT("DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE");
	case DXGI_ERROR_INVALID_CALL: return TEXT("DXGI_ERROR_INVALID_CALL");
	case DXGI_ERROR_MORE_DATA: return TEXT("DXGI_ERROR_MORE_DATA");
	case DXGI_ERROR_NAME_ALREADY_EXISTS: return TEXT("DXGI_ERROR_NAME_ALREADY_EXISTS");
	case DXGI_ERROR_NONEXCLUSIVE: return TEXT("DXGI_ERROR_NONEXCLUSIVE");
	case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE: return TEXT("DXGI_ERROR_NOT_CURRENTLY_AVAILABLE");
	case DXGI_ERROR_NOT_FOUND: return TEXT("DXGI_ERROR_NOT_FOUND");
	case DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED: return TEXT("DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED");
	case DXGI_ERROR_REMOTE_OUTOFMEMORY: return TEXT("DXGI_ERROR_REMOTE_OUTOFMEMORY");
	case DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE: return TEXT("DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE");
	case DXGI_ERROR_SDK_COMPONENT_MISSING: return TEXT("DXGI_ERROR_SDK_COMPONENT_MISSING");
	case DXGI_ERROR_SESSION_DISCONNECTED: return TEXT("DXGI_ERROR_SESSION_DISCONNECTED");
	case DXGI_ERROR_UNSUPPORTED: return TEXT("DXGI_ERROR_UNSUPPORTED");
	case DXGI_ERROR_WAIT_TIMEOUT: return TEXT("DXGI_ERROR_WAIT_TIMEOUT");
	case DXGI_ERROR_WAS_STILL_DRAWING: return TEXT("DXGI_ERROR_WAS_STILL_DRAWING");
	case E_FAIL: return TEXT("E_FAIL");
	case E_INVALIDARG: return TEXT("E_INVALIDARG");
	case E_OUTOFMEMORY: return TEXT("E_OUTOFMEMORY");
	case E_NOTIMPL: return TEXT("E_NOTIMPL");
	case E_ABORT: return TEXT("E_ABORT");
	case E_ACCESSDENIED: return TEXT("E_ACCESSDENIED");
	case E_HANDLE: return TEXT("E_HANDLE");
	case E_NOINTERFACE: return TEXT("E_NOINTERFACE");
	case E_POINTER: return TEXT("E_POINTER");
	case E_UNEXPECTED: return TEXT("E_UNEXPECTED");
	case S_FALSE: return TEXT("S_FALSE");
	case S_OK: return TEXT("S_OK");
	default:
		return TEXT("Unknown");
	}
}

FString ToString(const EFeatureLevel& FeatureLevel)
{
	switch (FeatureLevel)
	{
	ENUM_CASE(EFeatureLevel, None);
	ENUM_CASE(EFeatureLevel, FL_11_0);
	ENUM_CASE(EFeatureLevel, FL_11_1);
	ENUM_CASE(EFeatureLevel, FL_12_0);
	ENUM_CASE(EFeatureLevel, FL_12_1);
	ENUM_CASE(EFeatureLevel, FL_12_2);
	default:
		return TEXT("Unknown");
	}
}

FString ToString(const EShaderModel& ShaderModel)
{
	switch (ShaderModel)
	{
	ENUM_CASE(EShaderModel, None);
	ENUM_CASE(EShaderModel, SM_5_1);
	ENUM_CASE(EShaderModel, SM_6_0);
	ENUM_CASE(EShaderModel, SM_6_1);
	ENUM_CASE(EShaderModel, SM_6_2);
	ENUM_CASE(EShaderModel, SM_6_3);
	ENUM_CASE(EShaderModel, SM_6_4);
	ENUM_CASE(EShaderModel, SM_6_5);
	ENUM_CASE(EShaderModel, SM_6_6);
	ENUM_CASE(EShaderModel, SM_6_7);
	ENUM_CASE(EShaderModel, SM_6_8);
	ENUM_CASE(EShaderModel, SM_6_9);
	default:
		return TEXT("Unknown");
	}
}

FString ToString(const ERayTracingTier& RayTracingTier)
{
	switch (RayTracingTier)
	{
	ENUM_CASE(ERayTracingTier, None);
	ENUM_CASE(ERayTracingTier, Tier_1_0);
	ENUM_CASE(ERayTracingTier, Tier_1_1);
	default:
		return TEXT("Unknown");
	}
}

FString ToString(const EVariableShadingRateTier& VariableShadingRateTier)
{
	switch (VariableShadingRateTier)
	{
	ENUM_CASE(EVariableShadingRateTier, None);
	ENUM_CASE(EVariableShadingRateTier, Tier_1);
	ENUM_CASE(EVariableShadingRateTier, Tier_2);
	default:
		return TEXT("Unknown");
	}
}

FString ToString(const EMeshShaderTier& MeshShaderTier)
{
	switch (MeshShaderTier)
	{
	ENUM_CASE(EMeshShaderTier, None);
	ENUM_CASE(EMeshShaderTier, Tier_1);
	default:
		return TEXT("Unknown");
	}
}

FString ToString(const ECommandListType& CommandListType)
{
	switch (CommandListType)
	{
	ENUM_CASE(ECommandListType, None);
	ENUM_CASE(ECommandListType, Graphics);
	ENUM_CASE(ECommandListType, Compute);
	ENUM_CASE(ECommandListType, Copy);
	default:
		return TEXT("Unknown");
	}
}

#pragma endregion
