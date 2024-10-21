#include "pch.h"

#include "Renderwerk/RHI/Commands/CommandQueue.h"
#include "Renderwerk/RHI/Components/Device.h"

FCommandQueue::FCommandQueue(FDevice* InDevice, ECommandListType InType)
	: IDeviceChild(TEXT("CommandQueue"), InDevice), Type(InType)
{
	D3D12_COMMAND_QUEUE_DESC Description;
	Description.NodeMask = 0;
	Description.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	Description.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	Description.Type = static_cast<D3D12_COMMAND_LIST_TYPE>(InType);
	FD3DResult CreateResult = GetDeviceHandle()->CreateCommandQueue(&Description, IID_PPV_ARGS(&CommandQueue));
	D3D_CHECKM(CreateResult, "Failed to create command queue");
	D3D12_SET_NAME(CommandQueue, GetObjectName().c_str());
}

FCommandQueue::~FCommandQueue()
{
	CommandQueue.Reset();
}
