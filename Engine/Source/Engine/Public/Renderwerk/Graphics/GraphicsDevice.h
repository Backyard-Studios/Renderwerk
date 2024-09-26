#pragma once

#include "CommandList.h"
#include "CommandQueue.h"
#include "DescriptorHeap.h"
#include "GraphicsAdapter.h"

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Graphics/D3D12Include.h"

struct ENGINE_API FGraphicsDeviceDesc
{
	TSharedPtr<FGraphicsAdapter> Adapter;
};

class ENGINE_API FGraphicsDevice
{
public:
	FGraphicsDevice(const FGraphicsDeviceDesc& InDescription);
	~FGraphicsDevice();

	DELETE_COPY_AND_MOVE(FGraphicsDevice)

public:
	[[nodiscard]] TSharedPtr<FCommandQueue> CreateCommandQueue(const FCommandQueueDesc& InDescription);
	[[nodiscard]] TSharedPtr<FCommandList> CreateCommandList(const FCommandListDesc& InDescription);
	[[nodiscard]] TSharedPtr<FDescriptorHeap> CreateDescriptorHeap(const FDescriptorHeapDesc& InDescription);

public:
	[[nodiscard]] ComPtr<ID3D12Device14> GetHandle() const { return Device; }

	[[nodiscard]] TSharedPtr<FGraphicsAdapter> GetAdapter() const { return Description.Adapter; }

private:
	FGraphicsDeviceDesc Description;

	ComPtr<ID3D12Device14> Device;

#if RW_ENABLE_D3D12_DEBUG_LAYER
	ComPtr<ID3D12DebugDevice2> DebugDevice;
	ComPtr<ID3D12InfoQueue1> InfoQueue;
	DWORD InfoQueueCookie;
#endif
};
