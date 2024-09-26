#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Graphics/GraphicsCommon.h"

struct ENGINE_API FCommandQueueDesc
{
	D3D12_COMMAND_LIST_TYPE Type;
	D3D12_COMMAND_QUEUE_PRIORITY Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	D3D12_COMMAND_QUEUE_FLAGS Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
};

class ENGINE_API FCommandQueue : public IGraphicsDeviceChild
{
public:
	explicit FCommandQueue(FGraphicsDevice* InDevice, const FCommandQueueDesc& InDescription);
	~FCommandQueue() override;

	DELETE_COPY_AND_MOVE(FCommandQueue)

public:
	[[nodiscard]] ComPtr<ID3D12CommandQueue> GetHandle() const { return CommandQueue; }

private:
	FCommandQueueDesc Description;

	ComPtr<ID3D12CommandQueue> CommandQueue;
};
