#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include "Renderwerk/Graphics/D3D12Include.h"

class FGraphicsContext;
class FGraphicsDevice;

class ENGINE_API IGraphicsContextChild
{
public:
	IGraphicsContextChild(FGraphicsContext* InContext)
		: Context(InContext)
	{
	}

	virtual ~IGraphicsContextChild()
	{
		Context = nullptr;
	}

	DEFINE_DEFAULT_COPY_AND_MOVE(IGraphicsContextChild)

public:
	[[nodiscard]] FGraphicsContext* GetDevice() const { return Context; }

protected:
	FGraphicsContext* Context;
};

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
