#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/Graphics/GraphicsCommon.h"

class FGraphicsContext;
class FAdapter;

class RENDERWERK_API FDevice
{
public:
	FDevice(const TSharedPtr<FGraphicsContext>& InContext, const TSharedPtr<FAdapter>& InAdapter);
	~FDevice();

	DELETE_COPY_AND_MOVE(FDevice);

public:
	NODISCARD ComPtr<ID3D12Device14> GetHandle() const { return Device; }

	NODISCARD TSharedPtr<FGraphicsContext> GetContext() const { return Context; }
	NODISCARD TSharedPtr<FAdapter> GetAdapter() const { return Adapter; }

private:
	TSharedPtr<FGraphicsContext> Context;
	TSharedPtr<FAdapter> Adapter;

	ComPtr<ID3D12Device14> Device;

#if RW_ENABLE_GPU_DEBUGGING
	ComPtr<ID3D12InfoQueue1> InfoQueue;
	DWORD InfoQueueCookie;
#endif
};
