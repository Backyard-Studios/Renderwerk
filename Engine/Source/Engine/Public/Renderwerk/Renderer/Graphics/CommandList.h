#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/Graphics/GraphicsCommon.h"

class RENDERWERK_API FCommandList
{
public:
	FCommandList(const ComPtr<ID3D12Device14>& Device, const ECommandListType& InType);
	~FCommandList();

	DELETE_COPY_AND_MOVE(FCommandList);

public:
	void Reset() const;
	void Close() const;

public:
	NODISCARD ComPtr<ID3D12GraphicsCommandList10> GetHandle() const { return CommandList; }

	NODISCARD ECommandListType GetType() const { return Type; }

private:
	ECommandListType Type;

	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList10> CommandList;
};
