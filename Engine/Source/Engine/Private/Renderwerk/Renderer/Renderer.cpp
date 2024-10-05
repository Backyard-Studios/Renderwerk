#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Graphics/PipelineBuilder.h"
#include "Renderwerk/Graphics/ShaderCache.h"

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

	ShaderCompiler = MakeShared<FShaderCompiler>();
	DQ_ADD(ShaderCompiler);

	DeletionQueue.Add([=]() { FShaderCache::Clear(); });

	// TODO: REMOVE HARD CODED PATH
	ComPtr<IDxcBlob> RootBlob = ShaderCompiler->CompileRootSignature(FDirectories::GetShaderPath("DefaultRootSignature.hlsl"), EShaderConfiguration::Debug);
	CHECK_RESULT(Device->GetHandle()->CreateRootSignature(0, RootBlob->GetBufferPointer(), RootBlob->GetBufferSize(), IID_PPV_ARGS(&RootSignature)),
	             "Failed to create root signature")

	// TODO: REMOVE HARD CODED PATH
	FShaderCompilationDesc VertexShaderDesc = {};
	VertexShaderDesc.Stage = EShaderStage::Vertex;
	VertexShaderDesc.Configuration = EShaderConfiguration::Debug;
	FCompiledShader VertexShader = ShaderCompiler->CompileFromFile(FDirectories::GetShaderPath("Default.hlsl"), VertexShaderDesc);
	FShaderCache::Register("DefaultVertex", VertexShader);

	// TODO: REMOVE HARD CODED PATH
	FShaderCompilationDesc PixelShaderDesc = {};
	PixelShaderDesc.Stage = EShaderStage::Pixel;
	PixelShaderDesc.Configuration = EShaderConfiguration::Debug;
	FCompiledShader PixelShader = ShaderCompiler->CompileFromFile(FDirectories::GetShaderPath("Default.hlsl"), PixelShaderDesc);
	FShaderCache::Register("DefaultPixel", PixelShader);

	FPipelineBuilder PipelineBuilder(RootSignature);
	PipelineBuilder.SetVertexShader(VertexShader.GetBytecode())
	               .SetPixelShader(PixelShader.GetBytecode())
	               .AddInputElement({
		               .SemanticName = "POSITION", .SemanticIndex = 0, .Format = DXGI_FORMAT_R32G32B32_FLOAT, .InputSlot = 0, .AlignedByteOffset = 0,
		               .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, .InstanceDataStepRate = 0
	               }).AddInputElement({
		               .SemanticName = "COLOR", .SemanticIndex = 0, .Format = DXGI_FORMAT_R32G32B32A32_FLOAT, .InputSlot = 0, .AlignedByteOffset = 12,
		               .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, .InstanceDataStepRate = 0
	               });

	PipelineState = PipelineBuilder.Build(Device);

	struct ENGINE_API FVertex
	{
		float32 Position[3];
		float32 Color[4];
	};

	TVector<FVertex> Vertices = {
		{{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
		{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}
	};

	VertexBuffer = MakeShared<FGraphicsBuffer>(Device->GetResourceAllocator(), Vertices.size() * sizeof(FVertex), Vertices.size(), Vertices.data());
	DQ_ADD(VertexBuffer);

	TVector<uint32> Indices = {0, 1, 2};

	IndexBuffer = MakeShared<FGraphicsBuffer>(Device->GetResourceAllocator(), Indices.size() * sizeof(uint32), Indices.size(), Indices.data());
	DQ_ADD(IndexBuffer);

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
	{
		CommandList->TransitionResource(Swapchain->GetCurrentImage(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		CommandList->GetHandle()->SetPipelineState(PipelineState.Get());
		CommandList->GetHandle()->SetGraphicsRootSignature(RootSignature.Get());

		CommandList->SetRenderTargetView(Swapchain->GetCurrentRenderTargetViewHandle());
		CommandList->ClearRenderTargetView(Swapchain->GetCurrentRenderTargetViewHandle(), {0.1f, 0.1f, 0.1f, 1.0f});

		D3D12_VIEWPORT Viewport = {};
		Viewport.Width = static_cast<float32>(GetEngine()->GetMainWindow()->GetWindowState().ClientWidth);
		Viewport.Height = static_cast<float32>(GetEngine()->GetMainWindow()->GetWindowState().ClientHeight);
		Viewport.MaxDepth = 1.0f;
		CommandList->GetHandle()->RSSetViewports(1, &Viewport);

		D3D12_RECT ScissorRect = {};
		ScissorRect.right = static_cast<LONG>(GetEngine()->GetMainWindow()->GetWindowState().ClientWidth);
		ScissorRect.bottom = static_cast<LONG>(GetEngine()->GetMainWindow()->GetWindowState().ClientHeight);
		CommandList->GetHandle()->RSSetScissorRects(1, &ScissorRect);

		CommandList->GetHandle()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D12_VERTEX_BUFFER_VIEW VertexBufferView = VertexBuffer->GetVertexBufferView();
		CommandList->GetHandle()->IASetVertexBuffers(0, 1, &VertexBufferView);

		D3D12_INDEX_BUFFER_VIEW IndexBufferView = IndexBuffer->GetIndexBufferView();
		CommandList->GetHandle()->IASetIndexBuffer(&IndexBufferView);

		CommandList->GetHandle()->DrawIndexedInstanced(3, 1, 0, 0, 0);
	}
}

void FRenderer::EndFrame()
{
	RW_PROFILING_MARK_FUNCTION();

	FRenderFrame& Frame = RenderFrames.at(FrameIndex);

	TSharedPtr<FCommandList> CommandList = Frame.CommandList;
	{
		CommandList->TransitionResource(Swapchain->GetCurrentImage(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	}
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
