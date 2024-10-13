#include "pch.h"

#include "Renderwerk/Renderer/Device.h"

FDevice::FDevice(const TSharedPtr<FAdapter>& InAdapter, const FDeviceDesc& InDesc)
	: Adapter(InAdapter), Description(InDesc)
{
	TSharedPtr<FAdapter> D3D12Adapter = static_pointer_cast<FAdapter>(Adapter);
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

FDevice::~FDevice()
{
	SamplerHeap.reset();
	DepthStencilViewHeap.reset();
	RenderTargetViewHeap.reset();
	ShaderResourcesHeap.reset();
	Device.Reset();
	Adapter.reset();
}

TSharedPtr<FDescriptorHeap> FDevice::CreateDescriptorHeap(const FD3D12DescriptorHeapDesc& Desc) const
{
	return MakeShared<FDescriptorHeap>(Device.Get(), Desc);
}
