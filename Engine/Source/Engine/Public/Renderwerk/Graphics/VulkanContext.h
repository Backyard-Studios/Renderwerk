#pragma once

#include "VulkanTypes.h"

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/Result.h"

#include <vulkan/vulkan.h>

struct ENGINE_API FVulkanContextSettings
{
	bool bEnableValidation = false;
	bool bEnableDebugging = false;
	uint32 APIVersion = VK_API_VERSION_1_0;
	TVector<FVulkanRequireableComponent> RequestedLayers;
	TVector<FVulkanRequireableComponent> RequestedExtensions;
	VkAllocationCallbacks* Allocator = nullptr;
};

class ENGINE_API FVulkanContext
{
public:
	FVulkanContext(const FVulkanContextSettings& InSettings);
	~FVulkanContext();

	DELETE_COPY_AND_MOVE(FVulkanContext)

public:
	[[nodiscard]] FResult Initialize();
	void Destroy() const;

public:
	[[nodiscard]] VkInstance GetInstance() const { return Instance; }

	[[nodiscard]] TVector<FVulkanComponent> GetActivatedLayers() const { return ActivatedLayers; }
	[[nodiscard]] TVector<FVulkanComponent> GetActivatedExtensions() const { return ActivatedExtensions; }

private:
	FResult CreateInstance();
	FResult CreateDebugMessenger();

	FResult ValidateRequestedLayers(TVector<const char*>* OutValidatedLayers);
	FResult ValidateRequestedExtensions(TVector<const char*>* OutValidatedExtensions);

private:
	FVulkanContextSettings Settings;

	VkInstance Instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;

	TVector<FVulkanComponent> ActivatedLayers;
	TVector<FVulkanComponent> ActivatedExtensions;
};
