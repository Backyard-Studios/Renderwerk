#include "pch.h"

#include "Renderwerk/Graphics/CommandList.h"

#include "Renderwerk/Graphics/GraphicsDevice.h"

FCommandList::FCommandList(FGraphicsDevice* InDevice, const FCommandListDesc& InDescription)
	: IGraphicsDeviceChild(InDevice), Description(InDescription)
{
	ComPtr<ID3D12Device14> RawDevice = Device->GetHandle();
	CHECK_RESULT(RawDevice->CreateCommandAllocator(Description.Type, IID_PPV_ARGS(&CommandAllocator)), "Failed to create command allocator")
	CHECK_RESULT(RawDevice->CreateCommandList(0, Description.Type, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&CommandList)),
	             "Failed to create command list")
}

FCommandList::~FCommandList()
{
	CommandAllocator.Reset();
	CommandList.Reset();
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
