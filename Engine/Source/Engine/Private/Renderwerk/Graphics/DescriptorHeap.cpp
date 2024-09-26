#include "pch.h"

#include "Renderwerk/Graphics/DescriptorHeap.h"

#include "Renderwerk/Graphics/GraphicsDevice.h"

FDescriptorHandle::FDescriptorHandle()
	: Heap(nullptr), CPUHandle({}), GPUHandle({})
{
}

FDescriptorHandle::FDescriptorHandle(FDescriptorHeap* InHeap, const D3D12_CPU_DESCRIPTOR_HANDLE& InCPUHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& InGPUHandle)
	: Heap(InHeap), CPUHandle(InCPUHandle), GPUHandle(InGPUHandle)
{
}

bool8 FDescriptorHandle::IsShaderVisible() const
{
	return Heap->IsShaderVisible();
}

bool8 FDescriptorHandle::IsValid() const
{
	bool8 bValidHeap = Heap != nullptr;
	bool8 bValidCPU = CPUHandle.ptr != 0;
	bool8 bValidGPU = IsShaderVisible() ? GPUHandle.ptr != 0 : true;
	return bValidHeap && bValidCPU && bValidGPU;
}

FDescriptorHeap::FDescriptorHeap(FGraphicsDevice* InDevice, const FDescriptorHeapDesc& InDescription)
	: IGraphicsDeviceChild(InDevice), Description(InDescription)
{
	bIsShaderVisible = Description.Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || Description.Type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;

	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
	DescriptorHeapDesc.Flags = bIsShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DescriptorHeapDesc.NodeMask = 0;
	DescriptorHeapDesc.Type = Description.Type;
	DescriptorHeapDesc.NumDescriptors = Description.Capacity;

	CHECK_RESULT(Device->GetHandle()->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&Heap)), "Failed to create descriptor heap")

	DescriptorSize = Device->GetHandle()->GetDescriptorHandleIncrementSize(Description.Type);
	CPUStartHandle = Heap->GetCPUDescriptorHandleForHeapStart();
	GPUStartHandle = Heap->GetGPUDescriptorHandleForHeapStart();

	for (uint32 Index = 0; Index < Description.Capacity; ++Index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = CPUStartHandle;
		CPUHandle.ptr += Index * DescriptorSize;

		D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = GPUStartHandle;
		GPUHandle.ptr += Index * DescriptorSize;

		FreeHandles.push(FDescriptorHandle(this, CPUHandle, GPUHandle));
	}
}

FDescriptorHeap::~FDescriptorHeap()
{
	for (size64 Index = 0; Index < FreeHandles.size(); ++Index)
		FreeHandles.pop();
	Heap.Reset();
}

FDescriptorHandle FDescriptorHeap::Allocate()
{
	RW_DEBUG_ASSERT(!FreeHandles.empty(), "Descriptor heap is full")

	FScopedLock Lock(Mutex);
	FDescriptorHandle Handle = FreeHandles.front();
	FreeHandles.pop();
	AllocatedHandles++;
	return Handle;
}

void FDescriptorHeap::Free(const FDescriptorHandle& Handle)
{
	RW_DEBUG_ASSERT(Handle.Heap == this, "Invalid descriptor handle")

	FScopedLock Lock(Mutex);
	FreeHandles.push(Handle);
	AllocatedHandles--;
}
