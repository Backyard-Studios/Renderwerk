#pragma once

#include "Renderwerk/RHI/RHICommon.h"

class RENDERWERK_API FDevice : public IAdapterChild
{
public:
	FDevice(FAdapter* Adapter);
	~FDevice() override;

	DELETE_COPY_AND_MOVE(FDevice);

public:
	NODISCARD TSharedPtr<FCommandQueue> CreateCommandQueue(ECommandListType Type);

	/**
	 * @brief Function that forwards the capabilities of the adapter.
	 */
	NODISCARD
	const FAdapterCapabilities& GetCapabilities() const;

public:
	NODISCARD TComPtr<ID3D12Device14> GetHandle() const { return Device; }

	NODISCARD TSharedPtr<FCommandQueue> GetGraphicsQueue() const { return GraphicsQueue; }
	NODISCARD TSharedPtr<FCommandQueue> GetComputeQueue() const { return ComputeQueue; }
	NODISCARD TSharedPtr<FCommandQueue> GetCopyQueue() const { return CopyQueue; }

private:
	TComPtr<ID3D12Device14> Device;

#if RW_ENABLE_GPU_DEBUGGING
	TComPtr<ID3D12InfoQueue1> InfoQueue;
	DWORD InfoQueueCookie = 0;
#endif

	TSharedPtr<FCommandQueue> GraphicsQueue;
	TSharedPtr<FCommandQueue> ComputeQueue;
	TSharedPtr<FCommandQueue> CopyQueue;
};
