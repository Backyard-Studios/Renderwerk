#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/DataTypes/Containers.h"
#include "Renderwerk/DataTypes/String.h"
#include "Renderwerk/Engine/Subsystem.h"
#include "Renderwerk/Memory/SmartPointers.h"

class RENDERWERK_API FSubsystemManager
{
public:
	FSubsystemManager();
	~FSubsystemManager();

	DELETE_COPY_AND_MOVE(FSubsystemManager);

public:
	template <typename TSubsystem, typename = std::is_base_of<ISubsystem, TSubsystem>>
	NODISCARD TSharedPtr<TSubsystem> Get() const
	{
		const std::type_info& TypeInfo = typeid(TSubsystem);
		const FAnsiString TypeName = TypeInfo.name();
		DEBUG_ASSERTM(Subsystems.contains(TypeName), "Subsystem not registered");
		TSharedPtr<ISubsystem> Subsystem = Subsystems.at(TypeName);
		return std::static_pointer_cast<TSubsystem>(Subsystem);
	}

	template <typename TSubsystem, typename = std::is_base_of<ISubsystem, TSubsystem>>
	NODISCARD bool8 IsRegistered() const
	{
		const std::type_info& TypeInfo = typeid(TSubsystem);
		const FAnsiString TypeName = TypeInfo.name();
		return Subsystems.contains(TypeName);
	}

	template <typename TSubsystem, typename... TArguments, typename = std::is_base_of<ISubsystem, TSubsystem>>
	void Register(TArguments&&... Arguments)
	{
		RW_PROFILING_MARK_FUNCTION();

		const std::type_info& TypeInfo = typeid(TSubsystem);
		const FAnsiString TypeName = TypeInfo.name();
		DEBUG_ASSERTM(!Subsystems.contains(TypeName), "Subsystem already registered");
		TSharedPtr<ISubsystem> Subsystem = MakeShared<TSubsystem>(std::forward<TArguments>(Arguments)...);
		Subsystem->Initialize();
		Subsystems.insert({TypeName, Subsystem});
	}

	template <typename TSubsystem, typename = std::is_base_of<ISubsystem, TSubsystem>>
	void Unregister()
	{
		RW_PROFILING_MARK_FUNCTION();

		const std::type_info& TypeInfo = typeid(TSubsystem);
		const FAnsiString TypeName = TypeInfo.name();
		DEBUG_ASSERTM(Subsystems.contains(TypeName), "Subsystem not registered");
		TSharedPtr<ISubsystem> Subsystem = Subsystems.at(TypeName);
		Subsystem->Shutdown();
		Subsystem.reset();
		Subsystems.erase(TypeName);
	}

private:
	TMap<FAnsiString, TSharedPtr<ISubsystem>> Subsystems;
};
