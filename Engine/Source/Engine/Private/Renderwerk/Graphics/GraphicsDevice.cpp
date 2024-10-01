#include "pch.h"

#include "Renderwerk/Graphics/GraphicsDevice.h"

void InfoQueueCallback(D3D12_MESSAGE_CATEGORY Category, D3D12_MESSAGE_SEVERITY Severity, D3D12_MESSAGE_ID MessageId, const LPCSTR Description, void* Context)
{
	// RW_LOG_ERROR("D3D12 Message: {}", Description);
	printf("D3D12 Message: %s\n", Description);
}

std::string ToString(const EVariableRateShadingTier Tier)
{
	switch (Tier)
	{
	case EVariableRateShadingTier::NotSupported: return "Not Supported";
	case EVariableRateShadingTier::Tier1: return "Tier 1";
	case EVariableRateShadingTier::Tier2: return "Tier 2";
	default:
		return "Unknown";
	}
}

std::string ToString(const EMeshShaderTier Tier)
{
	switch (Tier)
	{
	case EMeshShaderTier::NotSupported: return "Not Supported";
	case EMeshShaderTier::Tier1: return "Tier 1";
	default:
		return "Unknown";
	}
}

std::string ToString(const ERayTracingTier Tier)
{
	switch (Tier)
	{
	case ERayTracingTier::NotSupported: return "Not Supported";
	case ERayTracingTier::Tier1_0: return "Tier 1.0";
	case ERayTracingTier::Tier1_1: return "Tier 1.1";
	default:
		return "Unknown";
	}
}

FGraphicsDevice::FGraphicsDevice(const FGraphicsDeviceDesc& InDescription)
	: Description(InDescription)
{
	TSharedPtr<FGraphicsAdapter>& Adapter = Description.Adapter;
	HRESULT CreateDeviceResult = D3D12CreateDevice(Adapter->GetHandle().Get(), Adapter->GetMaxSupportedFeatureLevel(), IID_PPV_ARGS(Device.GetAddressOf()));
	CHECK_RESULT(CreateDeviceResult, "Failed to create D3D12 device")

#if RW_ENABLE_D3D12_DEBUG_LAYER
	CHECK_RESULT(Device->QueryInterface(IID_PPV_ARGS(&DebugDevice)), "Failed to create D3D12 debug device")
	CHECK_RESULT(DebugDevice->QueryInterface(IID_PPV_ARGS(&InfoQueue)), "Failed to create D3D12 info queue")
	CHECK_RESULT(InfoQueue->RegisterMessageCallback(InfoQueueCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &InfoQueueCookie),
	             "Failed to register message callback")
#endif

	Capabilities = {};

	D3D12_FEATURE_DATA_D3D12_OPTIONS5 Options5 = {};
	CHECK_RESULT(Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &Options5, sizeof(Options5)), "Failed to check D3D12 options 5");
	Capabilities.RayTracingTier = static_cast<ERayTracingTier>(Options5.RaytracingTier);

	RW_LOG_INFO("Ray Tracing Tier: {}", ToString(Capabilities.RayTracingTier));

	D3D12_FEATURE_DATA_D3D12_OPTIONS6 Options6 = {};
	CHECK_RESULT(Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &Options6, sizeof(Options6)), "Failed to check D3D12 options 6");
	Capabilities.VariableRateShadingTier = static_cast<EVariableRateShadingTier>(Options6.VariableShadingRateTier);

	RW_LOG_INFO("Variable Rate Shading Tier: {}", ToString(Capabilities.VariableRateShadingTier));

	D3D12_FEATURE_DATA_D3D12_OPTIONS7 Options7 = {};
	CHECK_RESULT(Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &Options7, sizeof(Options7)), "Failed to check D3D12 options 7");
	Capabilities.MeshShaderTier = static_cast<EMeshShaderTier>(Options7.MeshShaderTier);

	RW_LOG_INFO("Mesh Shader Tier: {}", ToString(Capabilities.MeshShaderTier));

	D3D12MA::ALLOCATOR_DESC AllocatorDesc = {};
	AllocatorDesc.pDevice = Device.Get();
	AllocatorDesc.pAdapter = Adapter->GetHandle().Get();

	CHECK_RESULT(D3D12MA::CreateAllocator(&AllocatorDesc, &ResourceAllocator), "Failed to create D3D12 resource allocator")
}

FGraphicsDevice::~FGraphicsDevice()
{
	ResourceAllocator.Reset();
#if RW_ENABLE_D3D12_DEBUG_LAYER
	if (InfoQueue)
	{
		HRESULT UnregisterResult = InfoQueue->UnregisterMessageCallback(InfoQueueCookie);
		if (FAILED(UnregisterResult))
			RW_LOG_ERROR("Failed to unregister message callback: {}", FPlatform::GetResultHandleDescription(UnregisterResult));
	}
	InfoQueue.Reset();
	DebugDevice.Reset();
#endif
	Device.Reset();
}

TSharedPtr<FCommandQueue> FGraphicsDevice::CreateCommandQueue(const FCommandQueueDesc& InDescription)
{
	return MakeShared<FCommandQueue>(this, InDescription);
}

TSharedPtr<FCommandList> FGraphicsDevice::CreateCommandList(const FCommandListDesc& InDescription)
{
	return MakeShared<FCommandList>(this, InDescription);
}

TSharedPtr<FDescriptorHeap> FGraphicsDevice::CreateDescriptorHeap(const FDescriptorHeapDesc& InDescription)
{
	return MakeShared<FDescriptorHeap>(this, InDescription);
}

TSharedPtr<FFence> FGraphicsDevice::CreateFence()
{
	return MakeShared<FFence>(this);
}

ComPtr<ID3D12PipelineState> FGraphicsDevice::CreatePipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& Description) const
{
	ComPtr<ID3D12PipelineState> PipelineState;
	CHECK_RESULT(Device->CreateGraphicsPipelineState(&Description, IID_PPV_ARGS(&PipelineState)),
	             "Failed to create D3D12 pipeline state")
	return PipelineState;
}
