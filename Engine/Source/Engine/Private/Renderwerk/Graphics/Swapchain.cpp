#include "pch.h"

#include "Renderwerk/Graphics/Swapchain.h"

#include "Renderwerk/Graphics/GraphicsContext.h"
#include "Renderwerk/Graphics/GraphicsDevice.h"

FSwapchain::FSwapchain(FGraphicsContext* InContext, const FSwapchainDesc& InDescription)
	: IGraphicsContextChild(InContext), Description(InDescription)
{
	CreationFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	DXGI_SWAP_CHAIN_DESC1 SwapchainDesc;
	SwapchainDesc.Width = Description.Window->GetWindowState().ClientWidth;
	SwapchainDesc.Height = Description.Window->GetWindowState().ClientHeight;
	SwapchainDesc.Format = Description.Format;
	SwapchainDesc.Flags = CreationFlags;
	SwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
	SwapchainDesc.BufferCount = Description.BufferCount;
	SwapchainDesc.SampleDesc = Description.SampleDesc;
	SwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapchainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	SwapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	SwapchainDesc.Stereo = FALSE;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC FullscreenDesc = {};
	FullscreenDesc.Windowed = TRUE;

	ComPtr<IDXGIFactory6> Factory = Context->GetFactory();
	ComPtr<ID3D12CommandQueue> CommandQueue = Description.CommandQueue->GetHandle();
	HWND WindowHandle = Description.Window->GetHandle();

	ComPtr<IDXGISwapChain1> TempSwapchain;
	CHECK_RESULT(Factory->CreateSwapChainForHwnd(CommandQueue.Get(), WindowHandle, &SwapchainDesc, &FullscreenDesc, nullptr, &TempSwapchain),
	             "Failed to create swapchain")
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
	Swapchain->ResizeBuffers(Description.BufferCount, WindowState.ClientWidth, WindowState.ClientHeight, Description.Format, CreationFlags);
	AcquireImages();
}

ComPtr<ID3D12Resource2> FSwapchain::GetImage(const uint32 Index) const
{
	RW_DEBUG_ASSERT(Index < Images.size(), "Invalid image index")
	return Images.at(Index);
}

ComPtr<ID3D12Resource2> FSwapchain::GetCurrentImage() const
{
	return GetImage(GetCurrentImageIndex());
}

FDescriptorHandle FSwapchain::GetRenderTargetViewHandle(const uint32 Index) const
{
	RW_DEBUG_ASSERT(Index < RenderTargetViewHandles.size(), "Invalid render target view index")
	return RenderTargetViewHandles.at(Index);
}

FDescriptorHandle FSwapchain::GetCurrentRenderTargetViewHandle() const
{
	return GetRenderTargetViewHandle(GetCurrentImageIndex());
}

void FSwapchain::AcquireImages()
{
	Images.resize(Description.BufferCount);
	for (uint32 Index = 0; Index < Description.BufferCount; ++Index)
	{
		CHECK_RESULT(Swapchain->GetBuffer(Index, IID_PPV_ARGS(&Images.at(Index))), "Failed to acquire swapchain image")

		FDescriptorHandle RenderTargetViewHandle = Description.RenderTargetViewHeap->Allocate();
		Description.Device->GetHandle()->CreateRenderTargetView(Images.at(Index).Get(), nullptr, RenderTargetViewHandle.GetCPUHandle());
		RenderTargetViewHandles.push_back(RenderTargetViewHandle);
	}
}

void FSwapchain::ReleaseImages()
{
	for (uint32 Index = 0; Index < Description.BufferCount; ++Index)
	{
		Description.RenderTargetViewHeap->Free(RenderTargetViewHandles.at(Index));
		Images.at(Index).Reset();
	}
	Images.clear();
}

uint32 FSwapchain::GetCurrentImageIndex() const
{
	return Swapchain->GetCurrentBackBufferIndex();
}
