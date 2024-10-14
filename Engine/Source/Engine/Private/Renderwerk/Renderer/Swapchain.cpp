#include "pch.h"

#include "Renderwerk/Renderer/Swapchain.h"

FSwapchain::FSwapchain(const ComPtr<IDXGIFactory7>& Factory, const TSharedPtr<FDevice>& InDevice, const FSwapchainDesc& InDesc)
	: Device(InDevice), Description(InDesc)
{
	FWindowState WindowState = Description.Window->GetWindowState();

	DXGI_SWAP_CHAIN_DESC1 SwapchainDesc;
	SwapchainDesc.Width = WindowState.ClientWidth;
	SwapchainDesc.Height = WindowState.ClientHeight;
	SwapchainDesc.Format = Description.Format;
	SwapchainDesc.Stereo = FALSE;
	SwapchainDesc.SampleDesc = Description.SampleDesc;
	SwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
	SwapchainDesc.BufferCount = Description.BufferCount;
	SwapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	SwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapchainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	SwapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	CreationFlags = SwapchainDesc.Flags;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC FullscreenDesc = {};
	FullscreenDesc.Windowed = TRUE;

	ComPtr<IDXGISwapChain1> TempSwapchain;
	ID3D12CommandQueue* QueueHandle = Device->GetGraphicsQueue().Get();
	HRESULT CreateResult = Factory->CreateSwapChainForHwnd(QueueHandle, Description.Window->GetHandle(), &SwapchainDesc, &FullscreenDesc, nullptr, &TempSwapchain);
	CHECK_RESULT(CreateResult, "Failed to create swapchain")
	CHECK_RESULT(TempSwapchain.As(&Swapchain), "Failed to create swapchain")
	TempSwapchain.Reset();

	AcquireImages();
}

FSwapchain::~FSwapchain()
{
	ReleaseImages();
	Swapchain.Reset();
}

void FSwapchain::Present(const bool bUseVsync) const
{
	uint32 SyncInterval = bUseVsync ? 1 : 0;
	uint32 PresentFlags = bUseVsync ? 0 : DXGI_PRESENT_ALLOW_TEARING;
	CHECK_RESULT(Swapchain->Present(SyncInterval, PresentFlags), "Failed to present swapchain")
}

void FSwapchain::Resize()
{
	ReleaseImages();
	FWindowState WindowState = Description.Window->GetWindowState();
	HRESULT ResizeResult = Swapchain->ResizeBuffers(Description.BufferCount, WindowState.ClientWidth, WindowState.ClientHeight, Description.Format, CreationFlags);
	CHECK_RESULT(ResizeResult, "Failed to resize swapchain")
	AcquireImages();
}

uint32 FSwapchain::GetBackBufferIndex() const
{
	return Swapchain->GetCurrentBackBufferIndex();
}

ComPtr<ID3D12Resource2> FSwapchain::GetBackBuffer(const uint32 Index) const
{
	RW_DEBUG_ASSERT(Index < Description.BufferCount, "Index out of range")
	return BackBuffers.at(Index);
}

FDescriptorHandle FSwapchain::GetBackBufferHandle(const uint32 Index) const
{
	RW_DEBUG_ASSERT(Index < Description.BufferCount, "Index out of range")
	return BackBufferHandles.at(Index);
}

void FSwapchain::AcquireImages()
{
	BackBuffers.resize(Description.BufferCount);
	BackBufferHandles.resize(Description.BufferCount);
	for (uint32 Index = 0; Index < Description.BufferCount; ++Index)
	{
		CHECK_RESULT(Swapchain->GetBuffer(Index, IID_PPV_ARGS(&BackBuffers[Index])), "Failed to get swapchain buffer")
		BackBufferHandles[Index] = Device->GetRenderTargetViewHeap()->Allocate();
		Device->GetHandle()->CreateRenderTargetView(BackBuffers[Index].Get(), nullptr, BackBufferHandles[Index].GetCPUHandle());
	}
}

void FSwapchain::ReleaseImages()
{
	for (FDescriptorHandle& BackBufferHandle : BackBufferHandles)
		Device->GetRenderTargetViewHeap()->Free(BackBufferHandle);
	BackBufferHandles.clear();
	for (ComPtr<ID3D12Resource2>& BackBuffer : BackBuffers)
		BackBuffer.Reset();
	BackBuffers.clear();
}
