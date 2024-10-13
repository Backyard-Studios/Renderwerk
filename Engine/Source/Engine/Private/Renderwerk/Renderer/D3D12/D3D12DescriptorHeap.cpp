#include "pch.h"

#include "Renderwerk/Renderer/D3D12/D3D12DescriptorHeap.h"

std::string ToString(const EDescriptorType Type)
{
	switch (Type)
	{
	DEFINE_ENUM_STRING_CASE(EDescriptorType, ShaderResource);
	DEFINE_ENUM_STRING_CASE(EDescriptorType, RenderTargetView);
	DEFINE_ENUM_STRING_CASE(EDescriptorType, Sampler);
	DEFINE_ENUM_STRING_CASE(EDescriptorType, DepthStencilView);
	DEFINE_ENUM_STRING_CASE(EDescriptorType, None);
	default:
		return "Unknown";
	}
}

D3D12_DESCRIPTOR_HEAP_TYPE ToD3D12DescriptorHeapType(const EDescriptorType Type)
{
	switch (Type)
	{
	case EDescriptorType::ShaderResource: return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	case EDescriptorType::RenderTargetView: return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	case EDescriptorType::Sampler: return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	case EDescriptorType::DepthStencilView: return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	case EDescriptorType::None:
	default:
		return D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
	}
}

FDescriptorHandle::FDescriptorHandle()
{
}

FDescriptorHandle::FDescriptorHandle(FD3D12DescriptorHeap* InHeap, const D3D12_CPU_DESCRIPTOR_HANDLE& InCPUHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& InGPUHandle)
	: Heap(InHeap), CPUHandle(InCPUHandle), GPUHandle(InGPUHandle)
{
}

FDescriptorHandle::~FDescriptorHandle()
{
}

bool8 FDescriptorHandle::IsValid() const
{
	return Heap != nullptr && CPUHandle.ptr != 0;
}

bool8 FDescriptorHandle::IsShaderVisible() const
{
	return Heap->IsShaderVisible();
}

EDescriptorType FDescriptorHandle::GetType() const
{
	return Heap->GetType();
}

FD3D12DescriptorHeap::FD3D12DescriptorHeap(const ComPtr<ID3D12Device14>& Device, const FD3D12DescriptorHeapDesc& Desc)
	: Description(Desc)
{
	bIsShaderVisible = Description.Type == EDescriptorType::ShaderResource || Description.Type == EDescriptorType::Sampler;

	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
	DescriptorHeapDesc.Type = ToD3D12DescriptorHeapType(Description.Type);
	DescriptorHeapDesc.NumDescriptors = Description.Capacity;
	DescriptorHeapDesc.Flags = bIsShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT CreateResult = Device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&DescriptorHeap));
	CHECK_RESULT(CreateResult, "Failed to create descriptor heap")

	DescriptorSize = Device->GetDescriptorHandleIncrementSize(DescriptorHeapDesc.Type);
	CPUStartHandle = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	if (bIsShaderVisible)
		GPUStartHandle = DescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	for (uint32 Index = 0; Index < Description.Capacity; ++Index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = CPUStartHandle;
		CPUHandle.ptr += Index * DescriptorSize;

		D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = GPUStartHandle;
		if (bIsShaderVisible)
			GPUHandle.ptr += Index * DescriptorSize;

		FreeHandles.push(FDescriptorHandle(this, CPUHandle, GPUHandle));
	}
}

FD3D12DescriptorHeap::~FD3D12DescriptorHeap()
{
	for (size64 Index = 0; Index < FreeHandles.size(); ++Index)
		FreeHandles.pop();
	DescriptorHeap.Reset();
}

FDescriptorHandle FD3D12DescriptorHeap::Allocate()
{
	RW_DEBUG_ASSERT(!FreeHandles.empty(), "Descriptor heap is full")

	FScopedLock Lock(Mutex);
	FDescriptorHandle Handle = FreeHandles.front();
	FreeHandles.pop();
	AllocatedHandles++;
	return Handle;
}

void FD3D12DescriptorHeap::Free(const FDescriptorHandle& Handle)
{
	RW_DEBUG_ASSERT(Handle.Heap == this, "Invalid descriptor handle")

	FScopedLock Lock(Mutex);
	FreeHandles.push(Handle);
	AllocatedHandles--;
}

EDescriptorType FD3D12DescriptorHeap::GetType() const
{
	return Description.Type;
}
