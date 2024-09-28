#include "pch.h"

#include "Renderwerk/Graphics/GraphicsResource.h"

std::string ToString(const EGraphicsResourceType& Type)
{
	switch (Type)
	{
	DEFINE_ENUM_STRING_CASE(EGraphicsResourceType, Buffer);
	DEFINE_ENUM_STRING_CASE(EGraphicsResourceType, Texture);
	default:
		return "Unknown";
	}
}

IGraphicsResource::IGraphicsResource(const EGraphicsResourceType& InType)
	: Type(InType)
{
}

IGraphicsResource::~IGraphicsResource()
{
	Allocation.Reset();
	Resource.Reset();
}

FGraphicsBuffer::FGraphicsBuffer(const ComPtr<D3D12MA::Allocator>& ResourceAllocator, const size64 InSize, const size64 InCount, void* InData)
	: IGraphicsResource(EGraphicsResourceType::Buffer), Size(InSize), Count(InCount), Data(InData)
{
	Description = {};
	Description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	Description.Alignment = 0;
	Description.Width = Size;
	Description.Height = 1;
	Description.DepthOrArraySize = 1;
	Description.MipLevels = 1;
	Description.Format = DXGI_FORMAT_UNKNOWN;
	Description.SampleDesc.Count = 1;
	Description.SampleDesc.Quality = 0;
	Description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	Description.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12MA::ALLOCATION_DESC AllocationDesc = {};
	AllocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

	CHECK_RESULT(ResourceAllocator->CreateResource(&AllocationDesc, &Description, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &Allocation, IID_PPV_ARGS(&Resource)),
	             "Failed to create buffer resource")

	void* MappedData;
	CHECK_RESULT(Resource->Map(0, nullptr, &MappedData), "Failed to map buffer resource")
	FMemory::Copy(MappedData, Data, Size);
	Resource->Unmap(0, nullptr);
}

FGraphicsBuffer::~FGraphicsBuffer()
{
}

D3D12_VERTEX_BUFFER_VIEW FGraphicsBuffer::GetVertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView = {};
	VertexBufferView.BufferLocation = Resource->GetGPUVirtualAddress();
	VertexBufferView.SizeInBytes = static_cast<uint32>(Size);
	VertexBufferView.StrideInBytes = static_cast<uint32>(Size / Count);
	return VertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW FGraphicsBuffer::GetIndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW IndexBufferView = {};
	IndexBufferView.BufferLocation = Resource->GetGPUVirtualAddress();
	IndexBufferView.SizeInBytes = static_cast<uint32>(Size);
	IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	return IndexBufferView;
}

FGraphicsTexture::FGraphicsTexture(const ComPtr<D3D12MA::Allocator>& ResourceAllocator, const uint32 InWidth, const uint32 InHeight, const DXGI_FORMAT InFormat,
                                   const D3D12_RESOURCE_FLAGS InFlags)
	: IGraphicsResource(EGraphicsResourceType::Texture)
{
	Description = {};
	Description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	Description.Alignment = 0;
	Description.Width = InWidth;
	Description.Height = InHeight;
	Description.DepthOrArraySize = 1;
	Description.MipLevels = 1;
	Description.Format = InFormat;
	Description.SampleDesc.Count = 1;
	Description.SampleDesc.Quality = 0;
	Description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	Description.Flags = InFlags;

	D3D12MA::ALLOCATION_DESC AllocationDesc = {};
	AllocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

	CHECK_RESULT(ResourceAllocator->CreateResource(&AllocationDesc, &Description, D3D12_RESOURCE_STATE_COMMON, nullptr, &Allocation, IID_PPV_ARGS(&Resource)),
	             "Failed to create texture resource")
}

FGraphicsTexture::~FGraphicsTexture()
{
}
