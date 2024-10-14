#include "pch.h"

#include "Renderwerk/Renderer/ResourceAllocator.h"

std::string ToString(const EResourceType Type)
{
	switch (Type)
	{
	case EResourceType::Buffer: return "Buffer";
	case EResourceType::Texture: return "Texture";
	case EResourceType::None:
	default:
		return "None";
	}
}

FGraphicsResource::FGraphicsResource(const EResourceType& InType, const ComPtr<ID3D12Resource2>& InResource, const ComPtr<D3D12MA::Allocation>& InAllocation)
	: Type(InType), Resource(InResource), Allocation(InAllocation)
{
}

FGraphicsResource::~FGraphicsResource()
{
	Allocation.Reset();
	Resource.Reset();
}

bool8 FGraphicsResource::IsValid() const
{
	return Type != EResourceType::None && Resource && Allocation;
}

void FGraphicsResource::SetName(const std::string& Name) const
{
	RW_DEBUG_ASSERT(IsValid(), "Resource is not valid")
	Resource->SetName(ToWide(Name.c_str()).c_str());
}

FResourceAllocator::FResourceAllocator(const ComPtr<IDXGIAdapter4>& Adapter, const ComPtr<ID3D12Device14>& Device)
{
	D3D12MA::ALLOCATOR_DESC AllocatorDesc = {};
	AllocatorDesc.pAdapter = Adapter.Get();
	AllocatorDesc.pDevice = Device.Get();
	AllocatorDesc.Flags = D3D12MA::ALLOCATOR_FLAG_NONE;

	HRESULT CreateResult = CreateAllocator(&AllocatorDesc, &Allocator);
	CHECK_RESULT(CreateResult, "Failed to create allocator")
}

FResourceAllocator::~FResourceAllocator()
{
	Allocator.Reset();
}

FGraphicsResource FResourceAllocator::AllocateResource(const FResourceAllocationDesc& ResourceAllocationDesc) const
{
	EResourceType ResourceType = EResourceType::None;
	if (ResourceAllocationDesc.ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		ResourceType = EResourceType::Buffer;
	else if (ResourceAllocationDesc.ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
		ResourceType = EResourceType::Texture;

	ComPtr<ID3D12Resource2> Resource;
	ComPtr<D3D12MA::Allocation> Allocation;
	HRESULT AllocateResult = Allocator->CreateResource2(&ResourceAllocationDesc.AllocationDesc, &ResourceAllocationDesc.ResourceDesc, ResourceAllocationDesc.InitialState,
	                                                    nullptr, &Allocation, IID_PPV_ARGS(&Resource));
	CHECK_RESULT(AllocateResult, "Failed to allocate resource")
	return FGraphicsResource(ResourceType, Resource, Allocation);
}
