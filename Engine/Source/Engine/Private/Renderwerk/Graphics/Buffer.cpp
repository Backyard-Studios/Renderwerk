#include "pch.h"

#include "Renderwerk/Graphics/Buffer.h"
#include "Renderwerk/Graphics/GraphicsDevice.h"

FBuffer::FBuffer(const TSharedPtr<FResourceAllocator>& ResourceAllocator, const FBufferDesc& InDescription)
	: Description(InDescription), State(D3D12_RESOURCE_STATE_COMMON)
{
	bool8 bIsUploadType = Description.Type == EBufferType::Constant || Description.Type == EBufferType::Copy;

	D3D12MA::ALLOCATION_DESC AllocationDesc = {};
	AllocationDesc.HeapType = Description.bSupportsReadback ? D3D12_HEAP_TYPE_READBACK : (bIsUploadType ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT);

	D3D12_RESOURCE_DESC1 ResourceDesc = {};
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	ResourceDesc.Width = Description.Size;
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	ResourceDesc.SampleDesc.Count = 1;
	ResourceDesc.SampleDesc.Quality = 0;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	switch (Description.Type)
	{
	case EBufferType::Constant:
		SetState(D3D12_RESOURCE_STATE_GENERIC_READ);
		break;
	default:
		SetState(D3D12_RESOURCE_STATE_COMMON);
	}

	FResourceAllocationDesc ResourceAllocationDesc;
	ResourceAllocationDesc.AllocationDesc = AllocationDesc;
	ResourceAllocationDesc.ResourceDesc = ResourceDesc;
	ResourceAllocationDesc.InitialState = State;
	GPUResource = ResourceAllocator->Allocate(ResourceAllocationDesc);
}

FBuffer::~FBuffer()
{
	GPUResource.reset();
}

void FBuffer::Map(void** OutData) const
{
	HRESULT Result = GPUResource->GetHandle()->Map(0, nullptr, OutData);
	CHECK_RESULT(Result, "Failed to map buffer")
}

void FBuffer::Unmap() const
{
	GPUResource->GetHandle()->Unmap(0, nullptr);
}

void FBuffer::AllocateDescriptor(const TSharedPtr<FDescriptorHeap>& DescriptorHeap)
{
	DescriptorHandle = DescriptorHeap->Allocate();
}

void FBuffer::CreateShaderResourceView(const TSharedPtr<FGraphicsDevice>& Device) const
{
	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVDesc.Buffer = {};
	SRVDesc.Buffer.FirstElement = 0;
	SRVDesc.Buffer.NumElements = static_cast<uint32>(Description.Size / Description.Stride);
	SRVDesc.Buffer.StructureByteStride = static_cast<uint32>(Description.Stride);
	SRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	Device->GetHandle()->CreateShaderResourceView(GPUResource->GetHandle().Get(), &SRVDesc, DescriptorHandle.GetCPUHandle());
}

void FBuffer::CreateConstantBufferView(const TSharedPtr<FGraphicsDevice>& Device) const
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
	CBVDesc.BufferLocation = GPUResource->GetHandle()->GetGPUVirtualAddress();
	CBVDesc.SizeInBytes = static_cast<uint32>(Description.Size);
	Device->GetHandle()->CreateConstantBufferView(&CBVDesc, DescriptorHandle.GetCPUHandle());
}

void FBuffer::SetState(const D3D12_RESOURCE_STATES& InState)
{
	State = InState;
}
