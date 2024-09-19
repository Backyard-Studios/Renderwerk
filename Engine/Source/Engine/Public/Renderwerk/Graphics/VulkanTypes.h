#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"

/**
 * @brief Represents either a Vulkan layer or extension.
 */
struct ENGINE_API FVulkanComponent
{
	const char* Name;
	uint32 SpecVersion = 0;
	uint32 ImplementationVersion = SpecVersion; // Is the same as SpecVersion, when this struct is used for extensions.

	explicit FVulkanComponent(const char* Name)
		: Name(Name)
	{
	}
};

/**
 * @brief Represents either a Vulkan layer or extension, which can be marked as required.
 */
struct ENGINE_API FVulkanRequireableComponent : FVulkanComponent
{
	bool bIsRequired;

	FVulkanRequireableComponent(const char* Name, const bool BIsRequired)
		: FVulkanComponent(Name),
		  bIsRequired(BIsRequired)
	{
	}
};
