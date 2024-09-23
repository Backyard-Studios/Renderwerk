#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include "Renderwerk/Graphics/VulkanTypes.h"

#include "volk.h"

struct ENGINE_API FVulkanContextDesc
{
	bool bEnableValidationLayers = false;
};

struct ENGINE_API FVulkanContext
{
public:
	FVulkanContext(const FVulkanContextDesc& InDescription);
	~FVulkanContext();

	DEFINE_DEFAULT_COPY_AND_MOVE(FVulkanContext)

public:
	[[nodiscard]] FResult Initialize();
	void Destroy();

public:
	[[nodiscard]] VkAllocationCallbacks* GetAllocator() const { return Allocator; }
	[[nodiscard]] VkInstance GetInstance() const { return Instance; }

private:
	static VkAllocationCallbacks* CreateAllocator();

	static TVector<VkLayerProperties> AcquireAvailableLayers();
	static TVector<VkExtensionProperties> AcquireAvailableExtensions();

	static void ValidateLayers(const TVector<FVulkanRequireableComponent>& Layers, TVector<const char*>* OutValidatedLayers);
	static void ValidateExtensions(const TVector<FVulkanRequireableComponent>& Extensions, TVector<const char*>* OutValidatedExtensions);

private:
	FVulkanContextDesc Description;

	VkAllocationCallbacks* Allocator = nullptr;
	VkInstance Instance = VK_NULL_HANDLE;
};
