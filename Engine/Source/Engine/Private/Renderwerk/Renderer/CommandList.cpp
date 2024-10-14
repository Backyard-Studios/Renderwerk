#include "pch.h"

#include "Renderwerk/Renderer/CommandList.h"

FCommandList::FCommandList(const ComPtr<ID3D12Device14>& Device, const FCommandListDesc& InDesc)
	: Description(InDesc)
{
	D3D12_COMMAND_LIST_TYPE Type = ToD3D12CommandListType(InDesc.Type);

	HRESULT AllocatorCreateResult = Device->CreateCommandAllocator(Type, IID_PPV_ARGS(CommandAllocator.GetAddressOf()));
	CHECK_RESULT(AllocatorCreateResult, "Failed to create command allocator")

	HRESULT ListCreateResult = Device->CreateCommandList(0, Type, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(CommandList.GetAddressOf()));
	CHECK_RESULT(ListCreateResult, "Failed to create command list")
}

FCommandList::~FCommandList()
{
	CommandList.Reset();
	CommandAllocator.Reset();
}

ECommandListType FCommandList::GetType() const
{
	return Description.Type;
}

void FCommandList::Reset() const
{
	CHECK_RESULT(CommandAllocator->Reset(), "Failed to reset command allocator")
	CHECK_RESULT(CommandList->Reset(CommandAllocator.Get(), nullptr), "Failed to reset command list")
}

void FCommandList::Close() const
{
	CHECK_RESULT(CommandList->Close(), "Failed to close command list")
}
