#pragma once

#include "CommandQueue.h"
#include "DescriptorHeap.h"

#include "Renderwerk/Core/CoreDefinitions.h"

#include "Renderwerk/Graphics/GraphicsCommon.h"
#include "Renderwerk/Platform/Window.h"

struct ENGINE_API FSwapchainDesc
{
	TSharedPtr<FGraphicsDevice> Device;
	TSharedPtr<FCommandQueue> CommandQueue;
	TSharedPtr<FDescriptorHeap> RenderTargetViewHeap;
	TSharedPtr<FWindow> Window;
	DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uint32 BufferCount = 3;
	DXGI_SAMPLE_DESC SampleDesc = {1, 0};
};

class ENGINE_API FSwapchain : public IGraphicsContextChild
{
public:
	explicit FSwapchain(FGraphicsContext* InContext, const FSwapchainDesc& InDescription);
	~FSwapchain() override;

	DELETE_COPY_AND_MOVE(FSwapchain)

public:
	[[nodiscard]] uint32 GetCurrentImageIndex() const;

	void Present(bool bUseVsync = false) const;
	void Resize();

	[[nodiscard]] ComPtr<ID3D12Resource2> GetImage(uint32 Index) const;
	[[nodiscard]] ComPtr<ID3D12Resource2> GetCurrentImage() const;

	[[nodiscard]] FDescriptorHandle GetRenderTargetViewHandle(uint32 Index) const;
	[[nodiscard]] FDescriptorHandle GetCurrentRenderTargetViewHandle() const;

public:
	[[nodiscard]] ComPtr<IDXGISwapChain4> GetHandle() const { return Swapchain; }

	[[nodiscard]] DXGI_FORMAT GetFormat() const { return Description.Format; }

private:
	void AcquireImages();
	void ReleaseImages();

private:
	FSwapchainDesc Description;

	uint32 CreationFlags = 0;
	ComPtr<IDXGISwapChain4> Swapchain;

	TVector<ComPtr<ID3D12Resource2>> Images;
	TVector<FDescriptorHandle> RenderTargetViewHandles;
};
