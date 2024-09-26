#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Engine/Engine.h"

FRenderer::FRenderer(const FRendererSettings& InSettings)
	: Settings(InSettings)
{
	GraphicsContext = MakeShared<FGraphicsContext>();

	TVector<TSharedPtr<FGraphicsAdapter>> Adapters = GraphicsContext->GetAdapters();

	RW_LOG_INFO("Available Adapters:");
	for (TSharedPtr<FGraphicsAdapter>& AvailableAdapter : Adapters)
		RW_LOG_INFO("\t - {} [Vendor: {}, Type: {}]", AvailableAdapter->GetName(), ToString(AvailableAdapter->GetVendor()), ToString(AvailableAdapter->GetType()));

	FGraphicsAdapterRequirements AdapterRequirements = {};
	AdapterRequirements.MinFeatureLevel = D3D_FEATURE_LEVEL_12_2;
	AdapterRequirements.MinShaderModel = D3D_SHADER_MODEL_6_8;
	Adapter = FGraphicsContext::GetSuitableAdapter(Adapters, AdapterRequirements);
	RW_ASSERT_CRITICAL(Adapter, "Failed to find suitable adapter")
	RW_LOG_INFO("Selected Adapter: {} [{}, {}]", Adapter->GetName(), ToString(Adapter->GetMaxSupportedFeatureLevel()), ToString(Adapter->GetMaxSupportedShaderModel()));

	FGraphicsDeviceDesc DeviceDesc = {};
	DeviceDesc.Adapter = Adapter;
	Device = MakeShared<FGraphicsDevice>(DeviceDesc);

	FCommandQueueDesc CommandQueueDesc = {};
	CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	CommandQueue = Device->CreateCommandQueue(CommandQueueDesc);

	GetEngine()->GetMainWindow()->AppendTitle(std::format(" | D3D12 <{}>", ToString(Adapter->GetMaxSupportedShaderModel())));
}

FRenderer::~FRenderer()
{
	CommandQueue.reset();
	Device.reset();
	Adapter.reset();
	GraphicsContext.reset();
}
