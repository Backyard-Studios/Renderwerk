#include "pch.h"

#include "Renderwerk/Renderer/D3D12/D3D12Device.h"

#include "Renderwerk/Renderer/D3D12/D3D12Adapter.h"

FD3D12Device::FD3D12Device(const TSharedPtr<IAdapter>& InAdapter, const FDeviceDesc& InDesc)
	: IDevice(InAdapter, InDesc)
{
	TSharedPtr<FD3D12Adapter> D3D12Adapter = static_pointer_cast<FD3D12Adapter>(Adapter);
	HRESULT CreateResult = D3D12CreateDevice(D3D12Adapter->GetHandle().Get(), D3D12Adapter->GetMaxSupportedFeatureLevel(), IID_PPV_ARGS(Device.GetAddressOf()));
	CHECK_RESULT(CreateResult, "Failed to create D3D12 device")

	FD3D12DescriptorHeapDesc ShaderResourcesHeapDesc;
	ShaderResourcesHeapDesc.Type = EDescriptorType::ShaderResource;
	ShaderResourcesHeapDesc.Capacity = Description.MaxShaderResources;
	ShaderResourcesHeap = CreateDescriptorHeap(ShaderResourcesHeapDesc);

	FD3D12DescriptorHeapDesc RenderTargetViewHeapDesc;
	RenderTargetViewHeapDesc.Type = EDescriptorType::RenderTargetView;
	RenderTargetViewHeapDesc.Capacity = Description.MaxRenderTargets;
	RenderTargetViewHeap = CreateDescriptorHeap(RenderTargetViewHeapDesc);

	FD3D12DescriptorHeapDesc DepthStencilViewHeapDesc;
	DepthStencilViewHeapDesc.Type = EDescriptorType::DepthStencilView;
	DepthStencilViewHeapDesc.Capacity = Description.MaxDepthStencils;
	DepthStencilViewHeap = CreateDescriptorHeap(DepthStencilViewHeapDesc);

	FD3D12DescriptorHeapDesc SamplerHeapDesc;
	SamplerHeapDesc.Type = EDescriptorType::Sampler;
	SamplerHeapDesc.Capacity = Description.MaxSamplers;
	SamplerHeap = CreateDescriptorHeap(SamplerHeapDesc);
}

FD3D12Device::~FD3D12Device()
{
	SamplerHeap.reset();
	DepthStencilViewHeap.reset();
	RenderTargetViewHeap.reset();
	ShaderResourcesHeap.reset();
	Device.Reset();
}

TSharedPtr<FD3D12DescriptorHeap> FD3D12Device::CreateDescriptorHeap(const FD3D12DescriptorHeapDesc& Desc) const
{
	return MakeShared<FD3D12DescriptorHeap>(Device.Get(), Desc);
}
