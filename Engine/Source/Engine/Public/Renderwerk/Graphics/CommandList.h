#pragma once

#include "DescriptorHeap.h"

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Graphics/GraphicsCommon.h"

struct ENGINE_API FCommandListDesc
{
	D3D12_COMMAND_LIST_TYPE Type;
};

struct ENGINE_API FClearColor
{
	float32 Red = 0.0f;
	float32 Green = 0.0f;
	float32 Blue = 0.0f;
	float32 Alpha = 1.0f;
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

	void TransitionResource(const ComPtr<ID3D12Resource2>& Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) const;
	void SetRenderTargetView(const FDescriptorHandle& RenderTargetViewHandle) const;
	void ClearRenderTargetView(const FDescriptorHandle& RenderTargetViewHandle, const FClearColor& ClearColor = {}) const;

public:
	[[nodiscard]] ComPtr<ID3D12GraphicsCommandList10> GetHandle() const { return CommandList; }

private:
	FCommandListDesc Description;

	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList10> CommandList;
};
