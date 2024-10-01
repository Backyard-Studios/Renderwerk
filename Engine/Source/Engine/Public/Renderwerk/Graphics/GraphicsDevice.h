#pragma once

#include "CommandList.h"
#include "CommandQueue.h"
#include "DescriptorHeap.h"
#include "Fence.h"
#include "GraphicsAdapter.h"

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Graphics/D3D12Include.h"

enum class ENGINE_API EVariableRateShadingTier : uint8
{
	NotSupported = 0,
	Tier1 = 1,
	Tier2 = 2,
};

enum class ENGINE_API EMeshShaderTier : uint8
{
	NotSupported = 0,
	Tier1 = 10,
};

enum class ENGINE_API ERayTracingTier : uint8
{
	NotSupported = 0,
	Tier1_0 = 10,
	Tier1_1 = 11,
};

ENGINE_API std::string ToString(EVariableRateShadingTier Tier);
ENGINE_API std::string ToString(EMeshShaderTier Tier);
ENGINE_API std::string ToString(ERayTracingTier Tier);

struct ENGINE_API FDeviceCapabilities
{
	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_1_0_CORE;
	EVariableRateShadingTier VariableRateShadingTier = EVariableRateShadingTier::NotSupported;
	EMeshShaderTier MeshShaderTier = EMeshShaderTier::NotSupported;
	ERayTracingTier RayTracingTier = ERayTracingTier::NotSupported;
	D3D_SHADER_MODEL ShaderModel = D3D_SHADER_MODEL_NONE;
};

struct ENGINE_API FGraphicsDeviceDesc
{
	TSharedPtr<FGraphicsAdapter> Adapter;
};

class ENGINE_API FGraphicsDevice
{
public:
	FGraphicsDevice(const FGraphicsDeviceDesc& InDescription);
	~FGraphicsDevice();

	DELETE_COPY_AND_MOVE(FGraphicsDevice)

public:
	[[nodiscard]] TSharedPtr<FCommandQueue> CreateCommandQueue(const FCommandQueueDesc& InDescription);
	[[nodiscard]] TSharedPtr<FCommandList> CreateCommandList(const FCommandListDesc& InDescription);
	[[nodiscard]] TSharedPtr<FDescriptorHeap> CreateDescriptorHeap(const FDescriptorHeapDesc& InDescription);
	[[nodiscard]] TSharedPtr<FFence> CreateFence();
	[[nodiscard]] ComPtr<ID3D12PipelineState> CreatePipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& Description) const;

public:
	[[nodiscard]] ComPtr<ID3D12Device14> GetHandle() const { return Device; }

	[[nodiscard]] TSharedPtr<FGraphicsAdapter> GetAdapter() const { return Description.Adapter; }
	[[nodiscard]] FDeviceCapabilities GetCapabilities() const { return Capabilities; }
	[[nodiscard]] ComPtr<D3D12MA::Allocator> GetResourceAllocator() const { return ResourceAllocator; }

private:
	FGraphicsDeviceDesc Description;

	ComPtr<ID3D12Device14> Device;

#if RW_ENABLE_D3D12_DEBUG_LAYER
	ComPtr<ID3D12DebugDevice2> DebugDevice;
	ComPtr<ID3D12InfoQueue1> InfoQueue;
	DWORD InfoQueueCookie;
#endif

	FDeviceCapabilities Capabilities;

	ComPtr<D3D12MA::Allocator> ResourceAllocator;
};
