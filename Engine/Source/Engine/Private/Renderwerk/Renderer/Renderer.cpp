#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Engine/Engine.h"

FRenderer::FRenderer(const FRendererSettings& InSettings)
	: Settings(InSettings), FrameBuffer(InSettings.BufferCount)
{
	GraphicsContext = MakeShared<FGraphicsContext>();
	DQ_ADD(GraphicsContext);

	SetupAdapter();

	FGraphicsDeviceDesc DeviceDesc = {};
	DeviceDesc.Adapter = Adapter;
	Device = MakeShared<FGraphicsDevice>(DeviceDesc);
	DQ_ADD(Device);

	SetupCommandQueues();

	GetEngine()->GetMainWindow()->AppendTitle(std::format(" | D3D12 <{}>", ToString(Adapter->GetMaxSupportedShaderModel())));
}

FRenderer::~FRenderer()
{
	DeletionQueue.Flush();
}

void FRenderer::BeginFrame()
{
	RW_PROFILING_MARK_FUNCTION();

	FRenderFrame* Frame = FrameBuffer.GetCurrentBuffer();
}

void FRenderer::EndFrame()
{
	RW_PROFILING_MARK_FUNCTION();

	FRenderFrame* Frame = FrameBuffer.GetCurrentBuffer();

	FrameBuffer.AdvanceIndex();
}

void FRenderer::SetupAdapter()
{
	TVector<TSharedPtr<FGraphicsAdapter>> Adapters = GraphicsContext->GetAdapters();
	RW_LOG_INFO("Available Adapters:");
	for (TSharedPtr<FGraphicsAdapter>& AvailableAdapter : Adapters)
		RW_LOG_INFO("\t - {} [Vendor: {}, Type: {}]", AvailableAdapter->GetName(), ToString(AvailableAdapter->GetVendor()), ToString(AvailableAdapter->GetType()));

	FGraphicsAdapterRequirements AdapterRequirements = {};
	AdapterRequirements.MinFeatureLevel = D3D_FEATURE_LEVEL_12_2;
	AdapterRequirements.MinShaderModel = D3D_SHADER_MODEL_6_8;
	Adapter = FGraphicsContext::GetSuitableAdapter(Adapters, AdapterRequirements);
	RW_ASSERT_CRITICAL(Adapter, "Failed to find suitable adapter")
	DQ_ADD(Adapter);
	RW_LOG_INFO("Selected Adapter: {} [{}, {}]", Adapter->GetName(), ToString(Adapter->GetMaxSupportedFeatureLevel()), ToString(Adapter->GetMaxSupportedShaderModel()));
}

void FRenderer::SetupCommandQueues()
{
	FCommandQueueDesc DirectCommandQueueDesc = {};
	DirectCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	DirectCommandQueue = Device->CreateCommandQueue(DirectCommandQueueDesc);
	DQ_ADD(DirectCommandQueue);

	FCommandQueueDesc ComputeCommandQueueDesc = {};
	ComputeCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	ComputeCommandQueue = Device->CreateCommandQueue(ComputeCommandQueueDesc);
	DQ_ADD(ComputeCommandQueue);
}
