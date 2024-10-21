#pragma once

#include "Renderwerk/RHI/RHICommon.h"

class RENDERWERK_API FCommandQueue : public IDeviceChild
{
public:
	FCommandQueue(FDevice* InDevice, ECommandListType InType);
	~FCommandQueue() override;

	DELETE_COPY_AND_MOVE(FCommandQueue);

public:
	NODISCARD TComPtr<ID3D12CommandQueue> GetHandle() const { return CommandQueue; }

	NODISCARD ECommandListType GetType() const { return Type; }

private:
	ECommandListType Type;

	TComPtr<ID3D12CommandQueue> CommandQueue;
};
