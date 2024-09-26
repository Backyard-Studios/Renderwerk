#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include "Renderwerk/Graphics/D3D12Include.h"

class FGraphicsDevice;

class ENGINE_API IGraphicsDeviceChild
{
public:
	IGraphicsDeviceChild(FGraphicsDevice* InDevice)
		: Device(InDevice)
	{
	}

	virtual ~IGraphicsDeviceChild()
	{
		Device = nullptr;
	}

	DEFINE_DEFAULT_COPY_AND_MOVE(IGraphicsDeviceChild)

public:
	[[nodiscard]] FGraphicsDevice* GetDevice() const { return Device; }

protected:
	FGraphicsDevice* Device;
};
