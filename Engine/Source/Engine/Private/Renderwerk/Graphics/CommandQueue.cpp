#include "pch.h"

#include "Renderwerk/Graphics/CommandQueue.h"

#include "Renderwerk/Graphics/GraphicsDevice.h"

FCommandQueue::FCommandQueue(FGraphicsDevice* InDevice, const FCommandQueueDesc& InDescription)
	: IGraphicsDeviceChild(InDevice), Description(InDescription)
{
	D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
	CommandQueueDesc.Type = Description.Type;
	CommandQueueDesc.Priority = Description.Priority;
	CommandQueueDesc.Flags = Description.Flags;
	CommandQueueDesc.NodeMask = 0;

	CHECK_RESULT(InDevice->GetHandle()->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&CommandQueue)), "Failed to create command queue")
}

FCommandQueue::~FCommandQueue()
{
	CommandQueue.Reset();
}
