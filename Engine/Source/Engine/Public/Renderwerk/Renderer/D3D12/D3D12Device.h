#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/Device.h"
#include "Renderwerk/Renderer/D3D12/D3D12DescriptorHeap.h"
#include "Renderwerk/Renderer/D3D12/D3D12Include.h"

class ENGINE_API FD3D12Device : public IDevice
{
public:
	FD3D12Device(const TSharedPtr<IAdapter>& InAdapter, const FDeviceDesc& InDesc);
	~FD3D12Device() override;

	DELETE_COPY_AND_MOVE(FD3D12Device);

public:
	[[nodiscard]] TSharedPtr<FD3D12DescriptorHeap> CreateDescriptorHeap(const FD3D12DescriptorHeapDesc& Desc) const;

public:
	[[nodiscard]] ComPtr<ID3D12Device14> GetHandle() const { return Device; }

	[[nodiscard]] TSharedPtr<FD3D12DescriptorHeap> GetResourceViewsHeap() const { return ShaderResourcesHeap; }
	[[nodiscard]] TSharedPtr<FD3D12DescriptorHeap> GetRenderTargetViewHeap() const { return RenderTargetViewHeap; }
	[[nodiscard]] TSharedPtr<FD3D12DescriptorHeap> GetSamplerHeap() const { return SamplerHeap; }
	[[nodiscard]] TSharedPtr<FD3D12DescriptorHeap> GetDepthStencilViewHeap() const { return DepthStencilViewHeap; }

private:
	ComPtr<ID3D12Device14> Device;

	TSharedPtr<FD3D12DescriptorHeap> ShaderResourcesHeap;
	TSharedPtr<FD3D12DescriptorHeap> RenderTargetViewHeap;
	TSharedPtr<FD3D12DescriptorHeap> DepthStencilViewHeap;
	TSharedPtr<FD3D12DescriptorHeap> SamplerHeap;
};
