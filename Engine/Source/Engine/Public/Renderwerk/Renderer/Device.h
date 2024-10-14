#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/Adapter.h"
#include "Renderwerk/Renderer/CommandList.h"
#include "Renderwerk/Renderer/D3D12Include.h"
#include "Renderwerk/Renderer/DescriptorHeap.h"
#include "Renderwerk/Renderer/Fence.h"
#include "Renderwerk/Renderer/RendererTypes.h"

struct ENGINE_API FDeviceDesc
{
	uint32 MaxShaderResources = 10;
	uint32 MaxRenderTargets = 10;
	uint32 MaxSamplers = 10;
	uint32 MaxDepthStencils = 10;
};

class ENGINE_API FDevice
{
public:
	FDevice(const TSharedPtr<FAdapter>& InAdapter, const FDeviceDesc& InDesc);
	~FDevice();

	DELETE_COPY_AND_MOVE(FDevice);

public:
	[[nodiscard]] TSharedPtr<FDescriptorHeap> CreateDescriptorHeap(const FD3D12DescriptorHeapDesc& Desc) const;

	[[nodiscard]] TSharedPtr<FFence> CreateFence() const;

	[[nodiscard]] ComPtr<ID3D12CommandQueue> CreateCommandQueue(ECommandListType Type) const;
	[[nodiscard]] TSharedPtr<FCommandList> CreateCommandList(FCommandListDesc Description) const;

public:
	[[nodiscard]] ComPtr<ID3D12Device14> GetHandle() const { return Device; }

	[[nodiscard]] TSharedPtr<FAdapter> GetAdapter() const { return Adapter; }

	[[nodiscard]] ComPtr<ID3D12CommandQueue> GetGraphicsQueue() const { return GraphicsQueue; }
	[[nodiscard]] ComPtr<ID3D12CommandQueue> GetComputeQueue() const { return ComputeQueue; }

	[[nodiscard]] TSharedPtr<FDescriptorHeap> GetResourceViewsHeap() const { return ShaderResourcesHeap; }
	[[nodiscard]] TSharedPtr<FDescriptorHeap> GetRenderTargetViewHeap() const { return RenderTargetViewHeap; }
	[[nodiscard]] TSharedPtr<FDescriptorHeap> GetSamplerHeap() const { return SamplerHeap; }
	[[nodiscard]] TSharedPtr<FDescriptorHeap> GetDepthStencilViewHeap() const { return DepthStencilViewHeap; }

protected:
	TSharedPtr<FAdapter> Adapter;
	FDeviceDesc Description;

	ComPtr<ID3D12Device14> Device;

	ComPtr<ID3D12CommandQueue> GraphicsQueue;
	ComPtr<ID3D12CommandQueue> ComputeQueue;

	TSharedPtr<FDescriptorHeap> ShaderResourcesHeap;
	TSharedPtr<FDescriptorHeap> RenderTargetViewHeap;
	TSharedPtr<FDescriptorHeap> DepthStencilViewHeap;
	TSharedPtr<FDescriptorHeap> SamplerHeap;
};
