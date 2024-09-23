#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"

#include "volk.h"

/**
 * @brief Represents either a Vulkan layer or extension.
 */
struct ENGINE_API FVulkanComponent
{
	std::string Name;
	uint32 SpecVersion = 0;
	uint32 ImplementationVersion = SpecVersion; // Is the same as SpecVersion, when this struct is used for extensions.

	explicit FVulkanComponent(const std::string& InName)
		: Name(InName)
	{
	}
};

/**
 * @brief Represents either a Vulkan layer or extension, which can be marked as required.
 */
struct ENGINE_API FVulkanRequireableComponent : FVulkanComponent
{
	bool bIsRequired;

	FVulkanRequireableComponent(const std::string& InName, const bool InRequired)
		: FVulkanComponent(InName),
		  bIsRequired(InRequired)
	{
	}
};

/**
 * @brief Maps a list of Vulkan components to a list of their names.
 */
ENGINE_API INLINE TVector<const char*> VulkanMapComponents(const TVector<FVulkanRequireableComponent>& Components)
{
	TVector<const char*> Result;
	std::ranges::transform(Components, std::back_inserter(Result), [](const FVulkanRequireableComponent& Component) { return Component.Name.c_str(); });
	return Result;
}

/**
 * @brief Maps a list of Vulkan layers to a list of components
 */
ENGINE_API INLINE TVector<FVulkanComponent> VulkanMapLayersToComponents(const TVector<VkLayerProperties>& Layers)
{
	TVector<FVulkanComponent> Result;
	std::ranges::transform(Layers, std::back_inserter(Result), [](const VkLayerProperties& Layer)
	{
		FVulkanComponent Component(Layer.layerName);
		Component.SpecVersion = Layer.specVersion;
		Component.ImplementationVersion = Layer.implementationVersion;
		return Component;
	});
	return Result;
}

/**
 * @brief Maps a list of Vulkan extensions to a list of components
 */
ENGINE_API INLINE TVector<FVulkanComponent> VulkanMapExtensionsToComponents(const TVector<VkExtensionProperties>& Extensions)
{
	TVector<FVulkanComponent> Result;
	std::ranges::transform(Extensions, std::back_inserter(Result), [](const VkExtensionProperties& Extension)
	{
		FVulkanComponent Component(Extension.extensionName);
		Component.SpecVersion = Extension.specVersion;
		Component.ImplementationVersion = Extension.specVersion;
		return Component;
	});
	return Result;
}

/**
 * @brief Validates a list of components against a list of available components and returns the names of the valid components.
 */
ENGINE_API INLINE TVector<const char*> VulkanValidateComponents(const TVector<FVulkanRequireableComponent>& Components,
                                                                const TVector<FVulkanComponent>& AvailableComponents)
{
	TVector<const char*> Result;
	for (const FVulkanRequireableComponent& Component : Components)
	{
		bool bFound = false;
		for (const FVulkanComponent& AvailableComponent : AvailableComponents)
		{
			if (Component.Name == AvailableComponent.Name)
			{
				bFound = true;
				break;
			}
		}
		if (!bFound && Component.bIsRequired)
			RW_ASSERT(false, RESULT_FAILED, "Required component '{}' is not available", Component.Name);
		if (bFound)
			Result.push_back(Component.Name.c_str());
	}
	return Result;
}
