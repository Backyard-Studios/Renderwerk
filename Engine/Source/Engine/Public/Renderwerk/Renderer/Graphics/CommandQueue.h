#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/Graphics/GraphicsCommon.h"

class RENDERWERK_API FCommandQueue
{
public:
	FCommandQueue(const ComPtr<ID3D12Device14>& Device, const ECommandListType& InType);
	~FCommandQueue();

	DELETE_COPY_AND_MOVE(FCommandQueue);

public:
	NODISCARD ComPtr<ID3D12CommandQueue> GetHandle() const { return CommandQueue; }

	NODISCARD ECommandListType GetType() const { return Type; }

private:
	ECommandListType Type;

	ComPtr<ID3D12CommandQueue> CommandQueue;
};
