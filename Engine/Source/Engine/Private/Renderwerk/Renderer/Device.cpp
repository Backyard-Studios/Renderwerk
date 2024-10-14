#include "pch.h"

#include "Renderwerk/Renderer/Device.h"

FDevice::FDevice(const TSharedPtr<FAdapter>& InAdapter, const FDeviceDesc& InDesc)
	: Adapter(InAdapter), Description(InDesc)
{
	TSharedPtr<FAdapter> D3D12Adapter = static_pointer_cast<FAdapter>(Adapter);
	HRESULT DeviceCreateResult = D3D12CreateDevice(D3D12Adapter->GetHandle().Get(), D3D12Adapter->GetMaxSupportedFeatureLevel(), IID_PPV_ARGS(Device.GetAddressOf()));
	CHECK_RESULT(DeviceCreateResult, "Failed to create D3D12 device")

	GraphicsQueue = CreateCommandQueue(ECommandListType::Graphics);
	ComputeQueue = CreateCommandQueue(ECommandListType::Compute);
	CopyQueue = CreateCommandQueue(ECommandListType::Copy);

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

	ResourceAllocator = MakeShared<FResourceAllocator>(Adapter->GetHandle(), Device);
}

FDevice::~FDevice()
{
	ResourceAllocator.reset();
	SamplerHeap.reset();
	DepthStencilViewHeap.reset();
	RenderTargetViewHeap.reset();
	ShaderResourcesHeap.reset();
	ComputeQueue.Reset();
	GraphicsQueue.Reset();
	Device.Reset();
	Adapter.reset();
}

TSharedPtr<FDescriptorHeap> FDevice::CreateDescriptorHeap(const FD3D12DescriptorHeapDesc& Desc) const
{
	return MakeShared<FDescriptorHeap>(Device.Get(), Desc);
}

TSharedPtr<FFence> FDevice::CreateFence() const
{
	return MakeShared<FFence>(Device.Get());
}

ComPtr<ID3D12CommandQueue> FDevice::CreateCommandQueue(const ECommandListType Type) const
{
	D3D12_COMMAND_QUEUE_DESC ComputeQueueDesc;
	ComputeQueueDesc.NodeMask = 0;
	ComputeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ComputeQueueDesc.Type = ToD3D12CommandListType(Type);
	ComputeQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;

	ComPtr<ID3D12CommandQueue> Queue;
	HRESULT ComputeQueueCreateResult = Device->CreateCommandQueue(&ComputeQueueDesc, IID_PPV_ARGS(Queue.GetAddressOf()));
	CHECK_RESULT(ComputeQueueCreateResult, "Failed to create queue")
	return Queue;
}

TSharedPtr<FCommandList> FDevice::CreateCommandList(FCommandListDesc Description) const
{
	return MakeShared<FCommandList>(Device.Get(), Description);
}

ComPtr<ID3D12RootSignature> FDevice::CreateRootSignatureFromShader(const FShader& RootSignatureShader) const
{
	void* BufferPointer = RootSignatureShader.GetData()->GetBufferPointer();
	size64 BufferSize = RootSignatureShader.GetData()->GetBufferSize();

	ComPtr<ID3D12RootSignature> RootSignature;
	HRESULT CreateResult = Device->CreateRootSignature(0, BufferPointer, BufferSize, IID_PPV_ARGS(RootSignature.GetAddressOf()));
	CHECK_RESULT(CreateResult, "Failed to create root signature")
	return RootSignature;
}
