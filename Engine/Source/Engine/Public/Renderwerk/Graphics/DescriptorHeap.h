#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"

#include "Renderwerk/Graphics/GraphicsCommon.h"

class FDescriptorHeap;

struct ENGINE_API FDescriptorHandle
{
public:
	FDescriptorHandle();
	FDescriptorHandle(FDescriptorHeap* InHeap, const D3D12_CPU_DESCRIPTOR_HANDLE& InCPUHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& InGPUHandle);

public:
	[[nodiscard]] bool8 IsShaderVisible() const;
	[[nodiscard]] bool8 IsValid() const;

public:
	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return CPUHandle; }
	[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return GPUHandle; }

private:
	FDescriptorHeap* Heap;

	D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle;

	friend class FDescriptorHeap;
};

struct ENGINE_API FDescriptorHeapDesc
{
	D3D12_DESCRIPTOR_HEAP_TYPE Type;
	uint32 Capacity;
};

class ENGINE_API FDescriptorHeap : public IGraphicsDeviceChild
{
public:
	explicit FDescriptorHeap(FGraphicsDevice* InDevice, const FDescriptorHeapDesc& InDescription);
	~FDescriptorHeap() override;

	DELETE_COPY_AND_MOVE(FDescriptorHeap)

public:
	[[nodiscard]] FDescriptorHandle Allocate();
	void Free(const FDescriptorHandle& Handle);

public:
	[[nodiscard]] ComPtr<ID3D12DescriptorHeap> GetHandle() const { return Heap; }

	[[nodiscard]] uint32 GetCapacity() const { return Description.Capacity; }
	[[nodiscard]] uint32 GetUsage() const { return AllocatedHandles; }
	[[nodiscard]] bool8 IsShaderVisible() const { return bIsShaderVisible; }

private:
	FDescriptorHeapDesc Description;

	bool8 bIsShaderVisible = false;
	ComPtr<ID3D12DescriptorHeap> Heap;

	size64 DescriptorSize = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE CPUStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE GPUStartHandle;

	FMutex Mutex;
	uint32 AllocatedHandles = 0;
	TQueue<FDescriptorHandle> FreeHandles;
};
