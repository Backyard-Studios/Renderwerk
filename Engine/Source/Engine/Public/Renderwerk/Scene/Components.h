#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

struct ENGINE_API FIdComponent
{
	FGuid Id;

	FIdComponent() = default;

	explicit FIdComponent(const FGuid& Id)
		: Id(Id)
	{
	}
};

struct ENGINE_API FSceneSettingsComponent
{
	float32 ClearColorRed = 0.0f;
	float32 ClearColorGreen = 0.0f;
	float32 ClearColorBlue = 0.0f;

	FSceneSettingsComponent() = default;

	FSceneSettingsComponent(const float32 ClearColorRed, const float32 ClearColorGreen, const float32 ClearColorBlue)
		: ClearColorRed(ClearColorRed), ClearColorGreen(ClearColorGreen), ClearColorBlue(ClearColorBlue)
	{
	}
};
