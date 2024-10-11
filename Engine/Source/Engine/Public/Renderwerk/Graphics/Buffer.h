#pragma once

#include "DescriptorHeap.h"
#include "ResourceAllocator.h"

#include "Renderwerk/Core/CoreMinimal.h"

enum class ENGINE_API EBufferType : uint8
{
	Vertex,
	Index,
	Constant,
	Copy,
};

struct ENGINE_API FBufferDesc
{
	EBufferType Type;
	size64 Size;
	size64 Stride;
	bool8 bSupportsReadback;
};

class ENGINE_API FBuffer
{
public:
	FBuffer(const TSharedPtr<FResourceAllocator>& ResourceAllocator, const FBufferDesc& InDescription);
	~FBuffer();

	DELETE_COPY_AND_MOVE(FBuffer);

public:
	void Map(void** OutData) const;
	void Unmap() const;

	void AllocateDescriptor(const TSharedPtr<FDescriptorHeap>& DescriptorHeap);

	void CreateShaderResourceView(const TSharedPtr<FGraphicsDevice>& Device) const;
	void CreateConstantBufferView(const TSharedPtr<FGraphicsDevice>& Device) const;

	void SetState(const D3D12_RESOURCE_STATES& InState);

public:
	[[nodiscard]] D3D12_RESOURCE_STATES GetState() const { return State; }
	[[nodiscard]] TSharedPtr<FGPUResource> GetGPUResource() const { return GPUResource; }
	[[nodiscard]] FDescriptorHandle GetDescriptorHandle() const { return DescriptorHandle; }

private:
	FBufferDesc Description;

	D3D12_RESOURCE_STATES State;

	TSharedPtr<FGPUResource> GPUResource;

	FDescriptorHandle DescriptorHandle;
};
