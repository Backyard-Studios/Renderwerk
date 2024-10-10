#pragma once

#include "Entity.h"

#include "Renderwerk/Core/CoreMinimal.h"

#include "flecs.h"

template <typename... TComponents>
using FEntityQuery = flecs::query<TComponents...>;

class ENGINE_API FScene
{
public:
	FScene();
	FScene(const std::string& InName);
	~FScene();

	DELETE_COPY_AND_MOVE(FScene);

public:
	FEntity CreateEntity(const std::string& InName = "UnnamedEntity") const;
	void DestroyEntity(const FEntity& Entity) const;
	void DestroyEntity(FEntityId EntityId) const;

	FEntity GetEntity(FEntityId EntityId) const;

	template <typename... TComponents>
	FEntityQuery<TComponents...> Query() const
	{
		return World.query<TComponents...>();
	}

public:
	template <typename T>
	bool8 HasSceneComponent() const
	{
		return World.has<T>();
	}

	template <typename T, typename... TArguments>
	void AddSceneComponent(TArguments&&... Arguments)
	{
		return World.add<T>(T(std::forward<TArguments>(Arguments)...));
	}

	template <typename T, typename... TArguments>
	void SetSceneComponent(TArguments&&... Arguments)
	{
		return World.set<T>(T(std::forward<TArguments>(Arguments)...));
	}

	template <typename T>
	const T* GetSceneComponent()
	{
		return World.get<T>();
	}

public:
	[[nodiscard]] std::string GetName() const { return Name; }

private:
	std::string Name = "UnnamedScene";

	flecs::world World;
};
