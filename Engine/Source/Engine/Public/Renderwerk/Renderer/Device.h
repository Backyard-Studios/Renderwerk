#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/Adapter.h"

class ENGINE_API IDevice
{
public:
	IDevice(const TSharedPtr<IAdapter>& InAdapter)
		: Adapter(InAdapter)
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
};
