#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Platform/Window.h"
#include "Renderwerk/Renderer/D3D12Include.h"
#include "Renderwerk/Renderer/DescriptorHeap.h"
#include "Renderwerk/Renderer/Device.h"

struct ENGINE_API FSwapchainDesc
{
	TSharedPtr<FWindow> Window;
	DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uint32 BufferCount = 3;
	DXGI_SAMPLE_DESC SampleDesc = {1, 0};
};

class ENGINE_API FSwapchain
{
public:
	FSwapchain(const ComPtr<IDXGIFactory7>& Factory, const TSharedPtr<FDevice>& InDevice, const FSwapchainDesc& InDesc);
	~FSwapchain();

	DELETE_COPY_AND_MOVE(FSwapchain);

public:
	void Present(bool bUseVsync = false) const;
	void Resize();

	[[nodiscard]] uint32 GetBackBufferIndex() const;

	[[nodiscard]] ComPtr<ID3D12Resource2> GetBackBuffer(uint32 Index) const;
	[[nodiscard]] FDescriptorHandle GetBackBufferHandle(uint32 Index) const;

public:
	[[nodiscard]] ComPtr<IDXGISwapChain4> GetHandle() const { return Swapchain; }

private:
	void AcquireImages();
	void ReleaseImages();

private:
	TSharedPtr<FDevice> Device;
	FSwapchainDesc Description;

	uint32 CreationFlags = 0;
	ComPtr<IDXGISwapChain4> Swapchain;

	TVector<ComPtr<ID3D12Resource2>> BackBuffers;
	TVector<FDescriptorHandle> BackBufferHandles;
};
