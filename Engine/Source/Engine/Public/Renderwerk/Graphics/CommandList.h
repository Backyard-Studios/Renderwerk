#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Graphics/GraphicsCommon.h"

struct ENGINE_API FCommandListDesc
{
	D3D12_COMMAND_LIST_TYPE Type;
};

class ENGINE_API FCommandList : public IGraphicsDeviceChild
{
public:
	explicit FCommandList(FGraphicsDevice* InDevice, const FCommandListDesc& InDescription);
	~FCommandList() override;

	DELETE_COPY_AND_MOVE(FCommandList)

public:
	void Reset() const;
	void Close() const;

private:
	FCommandListDesc Description;

	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList10> CommandList;
};
