#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Graphics/GraphicsCommon.h"

class FCommandQueue;

class ENGINE_API FFence : public IGraphicsDeviceChild
{
public:
	explicit FFence(FGraphicsDevice* InDevice);
	~FFence() override;

	DELETE_COPY_AND_MOVE(FFence);

public:
	void Signal(const TSharedPtr<FCommandQueue>& CommandQueue);

	void WaitCPU(uint64 Timeout = INFINITE) const;
	void WaitGPU(const TSharedPtr<FCommandQueue>& CommandQueue, uint64 Value) const;

	[[nodiscard]] uint64 GetLastCompletedValue() const;
	[[nodiscard]] bool8 IsValueComplete(uint64 InValue) const;

public:
	[[nodiscard]] ComPtr<ID3D12Fence1> GetHandle() const { return Fence; }

	[[nodiscard]] uint64 GetLastSignaledValue() const { return LastSignaledValue; }

private:
	ComPtr<ID3D12Fence1> Fence;
	HANDLE FenceEvent;

	uint64 LastSignaledValue = 0;
};
