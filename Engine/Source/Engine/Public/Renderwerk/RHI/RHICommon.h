#pragma once

#pragma region Includes
// ReSharper disable CppUnusedIncludeDirective
#	include "Renderwerk/Core/CoreMinimal.h"
#	include "Renderwerk/Logging/LogCategory.h"
#	include "Renderwerk/Platform/Assert.h"
#	include "Renderwerk/RHI/RHIFwd.h"

#	include <d3d12.h>
#	include <d3d12sdklayers.h>
#	include <dxgi1_6.h>
#	include <dxgidebug.h>
#	include <wrl.h>
#	include <d3dx12/d3dx12.h>
// ReSharper enable CppUnusedIncludeDirective
#pragma endregion

#pragma region Result Assertions

#define D3D_CHECK(ResultPredicate) \
	{ \
		FD3DResult UNIQUE_NAME(Result) = ResultPredicate; \
		ASSERTR(SUCCEEDED(UNIQUE_NAME(Result)), UNIQUE_NAME(Result)) \
	}

#define D3D_CHECKM(ResultPredicate, ...) \
	{ \
		FD3DResult UNIQUE_NAME(Result) = ResultPredicate; \
		ASSERTRM(SUCCEEDED(UNIQUE_NAME(Result)), UNIQUE_NAME(Result), __VA_ARGS__) \
	}

#if RW_CONFIG_DEBUG || RW_CONFIG_DEVELOPMENT
#	define DEBUG_D3D_CHECK(ResultPredicate) D3D_CHECK(ResultPredicate)
#	define DEBUG_D3D_CHECKM(ResultPredicate, ...) D3D_CHECKM(ResultPredicate, __VA_ARGS__)
#else
#	define DEBUG_D3D_CHECK(ResultPredicate) { ResultPredicate; }
#	define DEBUG_D3D_CHECKM(ResultPredicate, ...) { ResultPredicate; }
#endif

#pragma endregion

#pragma region Definitions

#	if RW_ENABLE_GPU_DEBUGGING
/** @brief Set the name of a native d3d12 object for debugging purposes. */
#		define D3D12_SET_NAME(Object, Name) Object->SetName(TEXT(Name))

/** @brief Set the name of a RHI native object handle for debugging purposes. */
#		define RHI_SET_NAME(Object, Name) D3D12_SET_NAME(Object->GetHandle(), Name)
#else
/** @brief Set the name of a native d3d12 object for debugging purposes. */
#		define D3D12_SET_NAME(Object, Name)

/** @brief Set the name of a RHI native object handle for debugging purposes. */
#		define RHI_SET_NAME(Object, Name)
#	endif

#	if RW_ENABLE_GPU_DEBUGGING
#		define RW_GPU_DEBUGGING true
#	else
#		define RW_GPU_DEBUGGING false
#	endif

#pragma endregion

#pragma region Utility

DECLARE_LOG_CATEGORY(LogRHI, Trace);

// For easier access to the ComPtr type
template <typename T>
using TComPtr = Microsoft::WRL::ComPtr<T>;

using FD3DResult = const HRESULT;

#pragma endregion

#pragma region Enums

enum class RENDERWERK_API EAdapterType : uint8
{
	Unknown = 0,
	Software,
	Discrete,
};

// https://pcisig.com/membership/member-companies
enum class RENDERWERK_API EAdapterVendor : uint16
{
	Unknown = 0,
	NVIDIA = 0x10DE,
	AMD = 0x1002,
	Intel = 0x8086,
	Microsoft = 0x1414,
};

enum class RENDERWERK_API EFeatureLevel
{
	None = D3D_FEATURE_LEVEL_1_0_CORE,
	FL_11_0 = D3D_FEATURE_LEVEL_11_0,
	FL_11_1 = D3D_FEATURE_LEVEL_11_1,
	FL_12_0 = D3D_FEATURE_LEVEL_12_0,
	FL_12_1 = D3D_FEATURE_LEVEL_12_1,
	FL_12_2 = D3D_FEATURE_LEVEL_12_2,
};

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

enum class RENDERWERK_API ERaytracingTier
{
	None = D3D12_RAYTRACING_TIER_NOT_SUPPORTED,
	Tier_1_0 = D3D12_RAYTRACING_TIER_1_0,
	Tier_1_1 = D3D12_RAYTRACING_TIER_1_1,
};

enum class RENDERWERK_API EVariableShadingRateTier
{
	None = D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED,
	Tier_1 = D3D12_VARIABLE_SHADING_RATE_TIER_1,
	Tier_2 = D3D12_VARIABLE_SHADING_RATE_TIER_2,
};

enum class RENDERWERK_API EMeshShaderTier
{
	None = D3D12_MESH_SHADER_TIER_NOT_SUPPORTED,
	Tier_1 = D3D12_MESH_SHADER_TIER_1,
};

enum class RENDERWERK_API ECommandListType
{
	None = D3D12_COMMAND_LIST_TYPE_NONE,
	Graphics = D3D12_COMMAND_LIST_TYPE_DIRECT,
	Compute = D3D12_COMMAND_LIST_TYPE_COMPUTE,
	Copy = D3D12_COMMAND_LIST_TYPE_COPY,
};

#pragma endregion

#pragma region ToString Functions

RENDERWERK_API FString D3D12ResultToString(HRESULT Result);

RENDERWERK_API FString ToString(const EAdapterType& AdapterType);
RENDERWERK_API FString ToString(const EAdapterVendor& AdapterVendor);
RENDERWERK_API FString ToString(const EFeatureLevel& FeatureLevel);
RENDERWERK_API FString ToString(const EShaderModel& ShaderModel);
RENDERWERK_API FString ToString(const ERaytracingTier& RaytracingTier);
RENDERWERK_API FString ToString(const EVariableShadingRateTier& VariableShadingRateTier);
RENDERWERK_API FString ToString(const EMeshShaderTier& MeshShaderTier);
RENDERWERK_API FString ToString(const ECommandListType& CommandListType);

#pragma endregion

#pragma region Base Types

/**
 * @brief Base class for all RHI objects.
 * @note Is used to track rhi objects and their names for debugging purposes.
 */
class RENDERWERK_API IRHIObject
{
public:
	IRHIObject() = default;
	IRHIObject(FString&& InDefaultObjectName);
	virtual ~IRHIObject() = default;

public:
	virtual void SetObjectName(FString&& InObjectName);

public:
	NODISCARD FString GetObjectName() const { return ObjectName; }

private:
	FString ObjectName;
};

/**
 * @brief Base class for all RHI objects that are created from an adapter.
 * @note The adapter pointer is non owning. The lifetime is managed by the backend.
 */
class RENDERWERK_API IAdapterChild : public IRHIObject
{
public:
	IAdapterChild(FAdapter* InAdapter);
	IAdapterChild(FString&& InDefaultObjectName, FAdapter* InAdapter);
	// ReSharper disable once CppEnforceOverridingDestructorStyle
	virtual ~IAdapterChild() override = default;

public:
	NODISCARD FAdapter* GetAdapter() const { return Adapter; }

private:
	FAdapter* Adapter;
};

#pragma endregion
