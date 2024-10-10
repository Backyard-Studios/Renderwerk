#include "pch.h"

#include "Renderwerk/Scene/Scene.h"

#include "Renderwerk/Scene/Components.h"

FScene::FScene()
{
	SetSceneComponent<FSceneSettingsComponent>();
}

FScene::FScene(const std::string& InName)
	: Name(InName)
{
	SetSceneComponent<FSceneSettingsComponent>();
}

FScene::~FScene()
{
}

FEntity FScene::CreateEntity(const std::string& InName) const
{
	FEntity Entity = World.entity();
	Entity.SetComponent<FIdComponent>(NewGuid());
	Entity.SetComponent<FTagComponent>(InName);
	return Entity;
}

void FScene::DestroyEntity(const FEntity& Entity) const
{
	DestroyEntity(Entity.GetId());
}

void FScene::DestroyEntity(FEntityId EntityId) const
{
	World.entity(EntityId).destruct();
}

FEntity FScene::GetEntity(FEntityId EntityId) const
{
	return FEntity(World.entity(EntityId));
}
