#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Engine/Engine.h"

FRenderer::FRenderer(const FRendererSettings& InSettings)
	: Settings(InSettings)
{
	GraphicsContext = MakeShared<FGraphicsContext>();
	DQ_ADD(GraphicsContext);

	SetupAdapter();

	FGraphicsDeviceDesc DeviceDesc = {};
	DeviceDesc.Adapter = Adapter;
	Device = MakeShared<FGraphicsDevice>(DeviceDesc);
	DQ_ADD(Device);

	SetupCommandQueues();

	FDescriptorHeapDesc RenderTargetViewHeapDesc;
	RenderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RenderTargetViewHeapDesc.Capacity = Settings.BufferCount;
	RenderTargetViewHeap = Device->CreateDescriptorHeap(RenderTargetViewHeapDesc);
	DQ_ADD(RenderTargetViewHeap);

	FSwapchainDesc SwapchainDesc = {};
	SwapchainDesc.Device = Device;
	SwapchainDesc.CommandQueue = DirectCommandQueue;
	SwapchainDesc.RenderTargetViewHeap = RenderTargetViewHeap;
	SwapchainDesc.Window = GetEngine()->GetMainWindow(); // TODO: This is a temporary solution
	Swapchain = GraphicsContext->CreateSwapchain(SwapchainDesc);
	DQ_ADD(Swapchain);

	SetupRenderFrames();

	GetEngine()->GetMainWindow()->AppendTitle(std::format(" | D3D12 <{}>", ToString(Adapter->GetMaxSupportedShaderModel())));
}

FRenderer::~FRenderer()
{
	FlushFrames();
	DeletionQueue.Flush();
}

void FRenderer::BeginFrame()
{
	RW_PROFILING_MARK_FUNCTION();

	FRenderFrame& Frame = RenderFrames.at(FrameIndex);

	// Wait if the frame is still in flight
	Frame.Fence->WaitCPU();

	Frame.ImageIndex = Swapchain->GetCurrentImageIndex();

	TSharedPtr<FCommandList> CommandList = Frame.CommandList;
	CommandList->Reset();
}

void FRenderer::EndFrame()
{
	RW_PROFILING_MARK_FUNCTION();

	FRenderFrame& Frame = RenderFrames.at(FrameIndex);

	TSharedPtr<FCommandList> CommandList = Frame.CommandList;
	CommandList->Close();

	DirectCommandQueue->ExecuteCommandList(CommandList);
	Swapchain->Present();
	Frame.Fence->Signal(DirectCommandQueue);

	FrameIndex = (FrameIndex + 1) % Settings.BufferCount;
}

void FRenderer::Resize()
{
	RW_PROFILING_MARK_FUNCTION();
	FlushFrames();

	Swapchain->Resize();
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

void FRenderer::SetupRenderFrames()
{
	RenderFrames.resize(Settings.BufferCount);
	for (FRenderFrame& Frame : RenderFrames)
	{
		Frame.Fence = Device->CreateFence();

		FCommandListDesc CommandListDesc = {};
		CommandListDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		Frame.CommandList = Device->CreateCommandList(CommandListDesc);
		Frame.CommandList->Close();
	}
	DeletionQueue.Add([=]()
	{
		for (FRenderFrame& Frame : RenderFrames)
		{
			Frame.Fence.reset();
			Frame.CommandList.reset();
		}
		RenderFrames.clear();
	});
}

void FRenderer::Flush(const TSharedPtr<FCommandQueue>& InCommandQueue, const TSharedPtr<FFence>& InFence)
{
	InFence->Signal(InCommandQueue);
	InFence->WaitCPU();
}

void FRenderer::FlushFrames()
{
	for (FRenderFrame& Frame : RenderFrames)
		Flush(DirectCommandQueue, Frame.Fence);
}
