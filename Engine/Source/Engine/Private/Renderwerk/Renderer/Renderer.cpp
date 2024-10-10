#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Graphics/ShaderCache.h"
#include "Renderwerk/Scene/Components.h"

#include "imgui.h"
#include "backends/imgui_impl_dx12.h"
#include "backends/imgui_impl_win32.h"

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

	FDescriptorHeapDesc ImmediateShaderResourceViewHeapDesc;
	ImmediateShaderResourceViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	ImmediateShaderResourceViewHeapDesc.Capacity = 1;
	ImmediateShaderResourceViewHeap = Device->CreateDescriptorHeap(ImmediateShaderResourceViewHeapDesc);
	DQ_ADD(ImmediateShaderResourceViewHeap);

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

	SetupImGui();
	DeletionQueue.Add([=]() { CleanupImGui(); });

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

		D3D12_VIEWPORT Viewport = {};
		Viewport.Width = static_cast<float32>(GetEngine()->GetMainWindow()->GetWindowState().ClientWidth);
		Viewport.Height = static_cast<float32>(GetEngine()->GetMainWindow()->GetWindowState().ClientHeight);
		Viewport.MaxDepth = 1.0f;
		CommandList->GetHandle()->RSSetViewports(1, &Viewport);

		D3D12_RECT ScissorRect = {};
		ScissorRect.right = static_cast<LONG>(GetEngine()->GetMainWindow()->GetWindowState().ClientWidth);
		ScissorRect.bottom = static_cast<LONG>(GetEngine()->GetMainWindow()->GetWindowState().ClientHeight);
		CommandList->GetHandle()->RSSetScissorRects(1, &ScissorRect);

		BeginImGuiFrame();
	}
}

void FRenderer::EndFrame()
{
	RW_PROFILING_MARK_FUNCTION();

	FRenderFrame& Frame = RenderFrames.at(FrameIndex);

	TSharedPtr<FCommandList> CommandList = Frame.CommandList;
	{
		EndImGuiFrame();
		CommandList->TransitionResource(Swapchain->GetCurrentImage(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	}
	CommandList->Close();

	DirectCommandQueue->ExecuteCommandList(CommandList);

	ImGuiIO& IO = ImGui::GetIO();
	if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault(nullptr, CommandList->GetHandle().Get());
	}

	Swapchain->Present();
	Frame.Fence->Signal(DirectCommandQueue);

	FrameIndex = (FrameIndex + 1) % Settings.BufferCount;
}

void FRenderer::RenderScene(const TSharedPtr<FScene>& Scene)
{
	if (!Scene)
		return;
	RW_PROFILING_MARK_FUNCTION();

	FRenderFrame& Frame = RenderFrames.at(FrameIndex);
	TSharedPtr<FCommandList> CommandList = Frame.CommandList;

	const FSceneSettingsComponent* SceneSettings = Scene->GetSceneComponent<FSceneSettingsComponent>();

	CommandList->SetRenderTargetView(Swapchain->GetCurrentRenderTargetViewHandle());
	CommandList->ClearRenderTargetView(Swapchain->GetCurrentRenderTargetViewHandle(),
	                                   {SceneSettings->ClearColorRed, SceneSettings->ClearColorGreen, SceneSettings->ClearColorBlue, 1.0f});

	static bool8 ShowDemoWindow = true;
	if (ShowDemoWindow)
		ImGui::ShowDemoWindow(&ShowDemoWindow);
	GetEngine()->GetJobSystem()->DebugUI();
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

void FRenderer::SetupImGui() const
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& IO = ImGui::GetIO();
	IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(GetEngine()->GetMainWindow()->GetHandle());
	ImGui_ImplDX12_Init(Device->GetHandle().Get(), Settings.BufferCount, DXGI_FORMAT_R8G8B8A8_UNORM, ImmediateShaderResourceViewHeap->GetHandle().Get(),
	                    ImmediateShaderResourceViewHeap->GetCpuStartHandle(), ImmediateShaderResourceViewHeap->GetGpuStartHandle());
}

void FRenderer::CleanupImGui()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void FRenderer::BeginImGuiFrame()
{
	RW_PROFILING_MARK_FUNCTION();

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void FRenderer::EndImGuiFrame()
{
	RW_PROFILING_MARK_FUNCTION();

	ImGui::Render();

	FRenderFrame& Frame = RenderFrames.at(FrameIndex);
	TSharedPtr<FCommandList> CommandList = Frame.CommandList;
	CommandList->GetHandle()->SetDescriptorHeaps(1, ImmediateShaderResourceViewHeap->GetHandle().GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), CommandList->GetHandle().Get());
}
