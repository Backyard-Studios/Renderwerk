#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/Adapter.h"

struct ENGINE_API FDeviceDesc
{
	uint32 MaxShaderResources = 10;
	uint32 MaxRenderTargets = 10;
	uint32 MaxSamplers = 10;
	uint32 MaxDepthStencils = 10;
};

class ENGINE_API IDevice
{
public:
	IDevice(const TSharedPtr<IAdapter>& InAdapter, const FDeviceDesc& InDesc)
		: Adapter(InAdapter), Description(InDesc)
	{
	}

	virtual ~IDevice()
	{
		Adapter.reset();
	}

	DELETE_COPY_AND_MOVE(IDevice);

public:
	[[nodiscard]] TSharedPtr<IAdapter> GetAdapter() const { return Adapter; }

protected:
	TSharedPtr<IAdapter> Adapter;
	FDeviceDesc Description;
};
