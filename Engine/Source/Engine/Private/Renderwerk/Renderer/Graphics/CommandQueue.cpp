#include "pch.h"

#include "Renderwerk/Renderer/Graphics/CommandQueue.h"

FCommandQueue::FCommandQueue(const ComPtr<ID3D12Device14>& Device, const ECommandListType& InType)
	: Type(InType)
{
	D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
	CommandQueueDesc.NodeMask = 0;
	CommandQueueDesc.Type = static_cast<D3D12_COMMAND_LIST_TYPE>(Type);
	CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	HRESULT CreateResult = Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&CommandQueue));
	CHECK_RESULTM(CreateResult, "Failed to create command queue.");
}

FCommandQueue::~FCommandQueue()
{
	CommandQueue.Reset();
}
