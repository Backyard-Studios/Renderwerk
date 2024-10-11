#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Graphics/D3D12Include.h"

class FGraphicsDevice;

enum class ENGINE_API EGPUResourceType : uint8
{
	None,
	Buffer,
};

ENGINE_API std::string ToString(const EGPUResourceType& Type);

struct ENGINE_API FGPUResource
{
public:
	FGPUResource() = default;
	FGPUResource(const EGPUResourceType& InType, size64 InSize, const ComPtr<ID3D12Resource2>& InResource, const ComPtr<D3D12MA::Allocation>& InAllocation);
	~FGPUResource();

	DEFINE_DEFAULT_COPY_AND_MOVE(FGPUResource);

public:
	[[nodiscard]] bool8 IsValid() const;

public:
	[[nodiscard]] ComPtr<ID3D12Resource2> GetHandle() const { return Resource; }

	[[nodiscard]] EGPUResourceType GetType() const { return Type; }
	[[nodiscard]] ComPtr<D3D12MA::Allocation> GetAllocation() const { return Allocation; }

private:
	EGPUResourceType Type = EGPUResourceType::None;

	size64 Size = 0;
	ComPtr<ID3D12Resource2> Resource;
	ComPtr<D3D12MA::Allocation> Allocation;
};

struct ENGINE_API FResourceAllocationDesc
{
	D3D12MA::ALLOCATION_DESC AllocationDesc = {};
	D3D12_RESOURCE_DESC1 ResourceDesc = {};
	D3D12_RESOURCE_STATES InitialState = D3D12_RESOURCE_STATE_COMMON;
};

class ENGINE_API FResourceAllocator
{
public:
	FResourceAllocator(const TSharedPtr<FGraphicsDevice>& Device);
	~FResourceAllocator();

	DELETE_COPY_AND_MOVE(FResourceAllocator);

public:
	TSharedPtr<FGPUResource> Allocate(const FResourceAllocationDesc& Description) const;

private:
	ComPtr<D3D12MA::Allocator> Allocator;
};
