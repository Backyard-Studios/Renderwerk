#pragma once

#include "DescriptorHeap.h"

#include "Renderwerk/Core/CoreMinimal.h"
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

struct ENGINE_API FViewport
{
	uint32 Width = 0;
	uint32 Height = 0;
	float32 Depth = 1.0f;
};

struct ENGINE_API FScissor
{
	uint32 Left = 0;
	uint32 Top = 0;
	uint32 Right = 0;
	uint32 Bottom = 0;
};

class ENGINE_API FCommandList : public IGraphicsDeviceChild
{
public:
	explicit FCommandList(FGraphicsDevice* InDevice, const FCommandListDesc& InDescription);
	~FCommandList() override;

	DELETE_COPY_AND_MOVE(FCommandList);

public:
	void Reset() const;
	void Close() const;

	void TransitionResource(const ComPtr<ID3D12Resource2>& Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) const;
	void SetRenderTargetView(const FDescriptorHandle& RenderTargetViewHandle) const;
	void ClearRenderTargetView(const FDescriptorHandle& RenderTargetViewHandle, const FClearColor& ClearColor = {}) const;

	void SetViewport(const FViewport& Viewport) const;
	void SetViewportWithDefaultScissor(const FViewport& Viewport) const;
	void SetScissor(const FScissor& Scissor) const;
	void SetViewportAndScissor(const FViewport& Viewport, const FScissor& Scissor) const;

public:
	[[nodiscard]] ComPtr<ID3D12GraphicsCommandList10> GetHandle() const { return CommandList; }

private:
	FCommandListDesc Description;

	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList10> CommandList;
};
