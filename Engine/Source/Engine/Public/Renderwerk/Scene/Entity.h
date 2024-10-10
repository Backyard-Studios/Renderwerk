#pragma once

#include "Components.h"

#include "Renderwerk/Core/CoreMinimal.h"

#include "flecs.h"

using FEntityId = ecs_id_t;

struct ENGINE_API FEntity
{
public:
	FEntity()
	{
	}

	FEntity(const flecs::entity InEntity)
		: Entity(InEntity)
	{
	}

	~FEntity()
	{
	}

	DEFINE_DEFAULT_COPY_AND_MOVE(FEntity);

public:
	bool8 IsValid() const
	{
		return Entity.is_valid();
	}

	template <typename T>
	bool8 HasComponent() const
	{
		return Entity.has<T>();
	}

	template <typename T, typename... TArguments>
	void AddComponent(TArguments&&... Arguments)
	{
		Entity.add<T>(std::move(T(std::forward<TArguments>(Arguments)...)));
	}

	template <typename T, typename... TArguments>
	void SetComponent(TArguments&&... Arguments)
	{
		Entity.set(std::move(T(std::forward<TArguments>(Arguments)...)));
	}

	template <typename T>
	const T* GetComponent()
	{
		return Entity.get<T>();
	}

	FEntityId GetEntityId() const
	{
		return Entity.id();
	}

	FGuid GetGuid() const
	{
		RW_DEBUG_ASSERT(HasComponent<FIdComponent>(), "Entity does not have an IdComponent")
		const FIdComponent* IdComponent = Entity.get<FIdComponent>();
		return IdComponent->Id;
	}

	std::string GetName() const
	{
		return Entity.name().c_str();
	}

private:
	flecs::entity Entity;

	friend class FScene;
};
