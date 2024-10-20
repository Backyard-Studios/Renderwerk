#include "pch.h"

#include "Renderwerk/Engine/SubsystemManager.h"

FSubsystemManager::FSubsystemManager()
{
}

FSubsystemManager::~FSubsystemManager()
{
	for (TSharedPtr<ISubsystem>& Subsystem : Subsystems | std::views::values)
	{
		Subsystem->Shutdown();
		Subsystem.reset();
	}
	Subsystems.clear();
}
