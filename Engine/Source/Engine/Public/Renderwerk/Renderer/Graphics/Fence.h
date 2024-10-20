#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/Graphics/GraphicsCommon.h"

class FCommandQueue;

class RENDERWERK_API FFence
{
public:
	FFence(const ComPtr<ID3D12Device14>& Device);
	~FFence();

	DELETE_COPY_AND_MOVE(FFence);

public:
	void Signal(const TSharedPtr<FCommandQueue>& CommandQueue);

	void WaitCPU(uint64 Timeout) const;
	void WaitGPU(const TSharedPtr<FCommandQueue>& CommandQueue, uint64 Value) const;

	NODISCARD bool8 IsCompleted(uint64 Value) const;

	NODISCARD uint64 GetCompletedValue() const;

public:
	NODISCARD ComPtr<ID3D12Fence1> GetHandle() const { return Fence; }

	NODISCARD uint64 GetSignaledValue() const { return SignaledValue; }

private:
	uint64 SignaledValue;
	HANDLE FenceEvent;
	ComPtr<ID3D12Fence1> Fence;

	friend class FCommandQueue;
};
