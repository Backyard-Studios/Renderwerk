#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/D3D12/D3D12Include.h"

enum class ENGINE_API EDescriptorType : uint8
{
	None,
	ShaderResource,
	RenderTargetView,
	DepthStencilView,
	Sampler,
};

ENGINE_API std::string ToString(EDescriptorType Type);
ENGINE_API D3D12_DESCRIPTOR_HEAP_TYPE ToD3D12DescriptorHeapType(EDescriptorType Type);

// Forward declaration of FDescriptorHeap for use in FDescriptorHandle
class FD3D12DescriptorHeap;

struct ENGINE_API FDescriptorHandle
{
public:
	FDescriptorHandle();
	FDescriptorHandle(FD3D12DescriptorHeap* InHeap, const D3D12_CPU_DESCRIPTOR_HANDLE& InCPUHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& InGPUHandle);
	~FDescriptorHandle();

	DEFINE_DEFAULT_COPY_AND_MOVE(FDescriptorHandle);

public:
	[[nodiscard]] bool8 IsValid() const;

	[[nodiscard]] bool8 IsShaderVisible() const;

	[[nodiscard]] EDescriptorType GetType() const;

public:
	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return CPUHandle; }
	[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return GPUHandle; }

private:
	FD3D12DescriptorHeap* Heap = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = {};

	friend class FD3D12DescriptorHeap;
};

struct ENGINE_API FD3D12DescriptorHeapDesc
{
	EDescriptorType Type;
	uint32 Capacity;
};

class ENGINE_API FD3D12DescriptorHeap
{
public:
	FD3D12DescriptorHeap(const ComPtr<ID3D12Device14>& Device, const FD3D12DescriptorHeapDesc& Desc);
	~FD3D12DescriptorHeap();

	DELETE_COPY_AND_MOVE(FD3D12DescriptorHeap);

public:
	[[nodiscard]] FDescriptorHandle Allocate();
	void Free(const FDescriptorHandle& Handle);

	[[nodiscard]] EDescriptorType GetType() const;

public:
	[[nodiscard]] ComPtr<ID3D12DescriptorHeap> GetHandle() const { return DescriptorHeap; }

	[[nodiscard]] bool8 IsShaderVisible() const { return bIsShaderVisible; }

	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUStartHandle() const { return CPUStartHandle; }
	[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGPUStartHandle() const { return GPUStartHandle; }

private:
	FD3D12DescriptorHeapDesc Description;

	bool8 bIsShaderVisible = false;
	ComPtr<ID3D12DescriptorHeap> DescriptorHeap;

	size64 DescriptorSize = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE CPUStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE GPUStartHandle;

	FMutex Mutex;
	uint32 AllocatedHandles = 0;
	TQueue<FDescriptorHandle> FreeHandles;
};
