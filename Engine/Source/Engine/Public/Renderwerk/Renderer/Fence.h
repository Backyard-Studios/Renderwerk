#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/D3D12Include.h"

class ENGINE_API FFence
{
public:
	FFence(const ComPtr<ID3D12Device14>& Device);
	~FFence();

	DELETE_COPY_AND_MOVE(FFence);

public:
	void Signal(const ComPtr<ID3D12CommandQueue>& CommandQueue);

	void WaitCPU(uint64 Timeout = INFINITE) const;
	void WaitGPU(const ComPtr<ID3D12CommandQueue>& CommandQueue, uint64 Value) const;

	[[nodiscard]] uint64 GetLastCompletedValue() const;
	[[nodiscard]] bool8 IsValueComplete(uint64 InValue) const;

public:
	[[nodiscard]] ComPtr<ID3D12Fence1> GetHandle() const { return Fence; }

	[[nodiscard]] uint64 GetLastSignaledValue() const { return LastSignaledValue; }

private:
	ComPtr<ID3D12Fence1> Fence;
	HANDLE FenceEvent;

	uint64 LastSignaledValue;
};
