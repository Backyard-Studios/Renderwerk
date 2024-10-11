#include "pch.h"

#include "Renderwerk/Graphics/ResourceAllocator.h"
#include "Renderwerk/Graphics/GraphicsDevice.h"

std::string ToString(const EGPUResourceType& Type)
{
	switch (Type)
	{
	case EGPUResourceType::None: return "None";
	case EGPUResourceType::Buffer: return "Buffer";
	default:
		return "Unknown";
	}
}

FGPUResource::FGPUResource(const EGPUResourceType& InType, const size64 InSize, const ComPtr<ID3D12Resource2>& InResource,
                           const ComPtr<D3D12MA::Allocation>& InAllocation)
	: Type(InType), Size(InSize), Resource(InResource), Allocation(InAllocation)
{
}

FGPUResource::~FGPUResource()
{
	Resource.Reset();
	Allocation.Reset();
}

bool8 FGPUResource::IsValid() const
{
	return Type == EGPUResourceType::None || !Resource || !Allocation;
}

FResourceAllocator::FResourceAllocator(const TSharedPtr<FGraphicsDevice>& Device)
{
	D3D12MA::ALLOCATOR_DESC AllocatorDesc = {};
	AllocatorDesc.pDevice = Device->GetHandle().Get();
	AllocatorDesc.pAdapter = Device->GetAdapter()->GetHandle().Get();

	CHECK_RESULT(D3D12MA::CreateAllocator(&AllocatorDesc, &Allocator), "Failed to create D3D12 resource allocator")
}

FResourceAllocator::~FResourceAllocator()
{
}

TSharedPtr<FGPUResource> FResourceAllocator::Allocate(const FResourceAllocationDesc& Description) const
{
	EGPUResourceType Type = EGPUResourceType::None;
	if (Description.ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		Type = EGPUResourceType::Buffer;
	RW_DEBUG_ASSERT(Type != EGPUResourceType::None, "Invalid resource type")

	size64 Size = Description.ResourceDesc.Width * std::max(Description.ResourceDesc.Height, static_cast<UINT32>(1)) * std::max(
		Description.ResourceDesc.DepthOrArraySize, static_cast<UINT16>(1));

	ComPtr<D3D12MA::Allocation> Allocation;
	ComPtr<ID3D12Resource2> Resource;
	HRESULT AllocationResult = Allocator->CreateResource2(&Description.AllocationDesc, &Description.ResourceDesc, Description.InitialState, nullptr, &Allocation,
	                                                      IID_PPV_ARGS(&Resource));
	CHECK_RESULT(AllocationResult, "Failed to allocate resource")
	return MakeShared<FGPUResource>(Type, Size, Resource, Allocation);
}
