#include "pch.h"

#include "Renderwerk/Renderer/Graphics/CommandList.h"

FCommandList::FCommandList(const ComPtr<ID3D12Device14>& Device, const ECommandListType& InType)
	: Type(InType)
{
	D3D12_COMMAND_LIST_TYPE ListType = static_cast<D3D12_COMMAND_LIST_TYPE>(Type);
	const HRESULT CreateAllocatorResult = Device->CreateCommandAllocator(ListType, IID_PPV_ARGS(&CommandAllocator));
	DEBUG_CHECK_RESULTM(CreateAllocatorResult, "Failed to create command allocator");

	const HRESULT CreateCommandListResult = Device->CreateCommandList(0, ListType, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&CommandList));
	DEBUG_CHECK_RESULTM(CreateCommandListResult, "Failed to create command list");
}

FCommandList::~FCommandList()
{
	CommandList.Reset();
	CommandAllocator.Reset();
}

void FCommandList::Reset() const
{
	DEBUG_CHECK_RESULTM(CommandAllocator->Reset(), "Failed to reset command allocator")
	DEBUG_CHECK_RESULTM(CommandList->Reset(CommandAllocator.Get(), nullptr), "Failed to reset command list")
}

void FCommandList::Close() const
{
	DEBUG_CHECK_RESULTM(CommandList->Close(), "Failed to close command list")
}
