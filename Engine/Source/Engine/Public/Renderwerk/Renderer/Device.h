#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/Adapter.h"
#include "Renderwerk/Renderer/D3D12Include.h"

#include "Renderwerk/Renderer/DescriptorHeap.h"

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

public:
	[[nodiscard]] ComPtr<ID3D12Device14> GetHandle() const { return Device; }

	[[nodiscard]] TSharedPtr<FAdapter> GetAdapter() const { return Adapter; }

	[[nodiscard]] TSharedPtr<FDescriptorHeap> GetResourceViewsHeap() const { return ShaderResourcesHeap; }
	[[nodiscard]] TSharedPtr<FDescriptorHeap> GetRenderTargetViewHeap() const { return RenderTargetViewHeap; }
	[[nodiscard]] TSharedPtr<FDescriptorHeap> GetSamplerHeap() const { return SamplerHeap; }
	[[nodiscard]] TSharedPtr<FDescriptorHeap> GetDepthStencilViewHeap() const { return DepthStencilViewHeap; }

protected:
	TSharedPtr<FAdapter> Adapter;
	FDeviceDesc Description;

	ComPtr<ID3D12Device14> Device;

	TSharedPtr<FDescriptorHeap> ShaderResourcesHeap;
	TSharedPtr<FDescriptorHeap> RenderTargetViewHeap;
	TSharedPtr<FDescriptorHeap> DepthStencilViewHeap;
	TSharedPtr<FDescriptorHeap> SamplerHeap;
};
