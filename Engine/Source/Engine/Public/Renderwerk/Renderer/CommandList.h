#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/D3D12Include.h"
#include "Renderwerk/Renderer/RendererTypes.h"

struct ENGINE_API FCommandListDesc
{
	ECommandListType Type;
};

class ENGINE_API FCommandList
{
public:
	FCommandList(const ComPtr<ID3D12Device14>& Device, const FCommandListDesc& InDesc);
	~FCommandList();

	DELETE_COPY_AND_MOVE(FCommandList);

public:
	[[nodiscard]] ECommandListType GetType() const;

	void Reset() const;
	void Close() const;

public:
	[[nodiscard]] ComPtr<ID3D12GraphicsCommandList10> GetHandle() const { return CommandList; }

private:
	FCommandListDesc Description;

	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList10> CommandList;
};
