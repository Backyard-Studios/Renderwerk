﻿#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

class RENDERWERK_API ISubsystem
{
public:
	ISubsystem() = default;
	virtual ~ISubsystem() = default;

	DELETE_COPY_AND_MOVE(ISubsystem);

private:
	virtual void Initialize()
	{
	}

	virtual void Shutdown()
	{
	}

private:
	friend class FSubsystemManager;
};
