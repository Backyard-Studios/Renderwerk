#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/D3D12Include.h"

enum class ENGINE_API EResourceType : uint8
{
	None,
	Buffer,
	Texture,
};

ENGINE_API std::string ToString(EResourceType Type);

struct ENGINE_API FGraphicsResource
{
public:
	FGraphicsResource() = default;
	FGraphicsResource(const EResourceType& InType, const ComPtr<ID3D12Resource2>& InResource, const ComPtr<D3D12MA::Allocation>& InAllocation);
	~FGraphicsResource();

	DEFINE_DEFAULT_COPY_AND_MOVE(FGraphicsResource);

public:
	[[nodiscard]] bool8 IsValid() const;

	void SetName(const std::string& Name) const;

public:
	[[nodiscard]] ComPtr<ID3D12Resource2> GetHandle() const { return Resource; }

	[[nodiscard]] EResourceType GetType() const { return Type; }
	[[nodiscard]] ComPtr<D3D12MA::Allocation> GetAllocation() const { return Allocation; }

private:
	EResourceType Type = EResourceType::None;
	ComPtr<ID3D12Resource2> Resource;
	ComPtr<D3D12MA::Allocation> Allocation;
};

struct ENGINE_API FResourceAllocationDesc
{
	D3D12_RESOURCE_DESC1 ResourceDesc = {};
	D3D12MA::ALLOCATION_DESC AllocationDesc = {};
	D3D12_RESOURCE_STATES InitialState = D3D12_RESOURCE_STATE_COMMON;
};

class ENGINE_API FResourceAllocator
{
public:
	FResourceAllocator(const ComPtr<IDXGIAdapter4>& Adapter, const ComPtr<ID3D12Device14>& Device);
	~FResourceAllocator();

	DELETE_COPY_AND_MOVE(FResourceAllocator);

public:
	FGraphicsResource AllocateResource(const FResourceAllocationDesc& ResourceAllocationDesc) const;

private:
	ComPtr<D3D12MA::Allocator> Allocator;
};
