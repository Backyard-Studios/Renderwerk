#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include <vulkan/vulkan.h>

#include "VulkanTypes.h"

class FVulkanContext;

struct ENGINE_API FVulkanQueueData
{
	uint32 GraphicsQueueIndex = UINT32_MAX;
	uint32 ComputeQueueIndex = UINT32_MAX;
	uint32 TransferQueueIndex = UINT32_MAX;
	uint32 PresentQueueIndex = UINT32_MAX;

	[[nodiscard]] bool8 IsValid() const
	{
		return GraphicsQueueIndex != UINT32_MAX && ComputeQueueIndex != UINT32_MAX && TransferQueueIndex != UINT32_MAX && PresentQueueIndex != UINT32_MAX;
	}

	[[nodiscard]] bool8 IsGraphicsSharingWithPresent() const { return GraphicsQueueIndex == PresentQueueIndex; }
	[[nodiscard]] bool8 IsGraphicsQueueDedicated() const { return GraphicsQueueIndex != ComputeQueueIndex && GraphicsQueueIndex != TransferQueueIndex; }
};

struct ENGINE_API FVulkanQueueRequirements
{
	bool8 bRequireDedicatedGraphicsQueue = true; // Also used for present queue (if supported)
	bool8 bUseBestQueueForTransport = true;
};

struct ENGINE_API FVulkanSurfaceProperties
{
	VkSurfaceCapabilitiesKHR Capabilities = {};
	TVector<VkSurfaceFormatKHR> Formats;
	TVector<VkPresentModeKHR> PresentModes;
};

class ENGINE_API FVulkanAdapter
{
public:
	FVulkanAdapter(const VkPhysicalDevice& InPhysicalDevice);
	~FVulkanAdapter();

	DELETE_COPY_AND_MOVE(FVulkanAdapter)

public:
	FResult Initialize();

	FResult QueryQueueData(const VkSurfaceKHR& Surface, const FVulkanQueueRequirements& Requirements, FVulkanQueueData* OutQueueData) const;
	FResult QuerySurfaceProperties(const VkSurfaceKHR& Surface, FVulkanSurfaceProperties* OutSurfaceProperties) const;

	bool8 SupportsLayers(const TVector<FVulkanRequireableComponent>& Layers) const;
	bool8 SupportsExtensions(const TVector<FVulkanRequireableComponent>& Extensions) const;

	bool8 SupportsFeatures(const VkPhysicalDeviceFeatures& InFeatures) const;
	bool8 SupportsFeatures11(const VkPhysicalDeviceVulkan11Features& InFeatures) const;
	bool8 SupportsFeatures12(const VkPhysicalDeviceVulkan12Features& InFeatures) const;
	bool8 SupportsFeatures13(const VkPhysicalDeviceVulkan13Features& InFeatures) const;

public:
	[[nodiscard]] VkPhysicalDevice GetHandle() const { return PhysicalDevice; }

	[[nodiscard]] VkPhysicalDeviceProperties GetDeviceProperties() const { return DeviceProperties; }
	[[nodiscard]] VkPhysicalDeviceMemoryProperties GetMemoryProperties() const { return MemoryProperties; }

	[[nodiscard]] VkPhysicalDeviceFeatures GetFeatures() const { return Features; }
	[[nodiscard]] VkPhysicalDeviceVulkan11Features GetFeatures11() const { return Features11; }
	[[nodiscard]] VkPhysicalDeviceVulkan12Features GetFeatures12() const { return Features12; }
	[[nodiscard]] VkPhysicalDeviceVulkan13Features GetFeatures13() const { return Features13; }

private:
	VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;

	VkPhysicalDeviceProperties DeviceProperties = {};
	VkPhysicalDeviceMemoryProperties MemoryProperties = {};

	VkPhysicalDeviceFeatures Features = {};
	VkPhysicalDeviceVulkan11Features Features11 = {};
	VkPhysicalDeviceVulkan12Features Features12 = {};
	VkPhysicalDeviceVulkan13Features Features13 = {};
};

struct ENGINE_API FVulkanAdapterRequirements
{
	uint32 MinAPIVersion = VK_API_VERSION_1_0;
	bool bRequireDiscreteGPU = true;
	TVector<FVulkanRequireableComponent> RequiredLayers;
	TVector<FVulkanRequireableComponent> RequiredExtensions;
	VkPhysicalDeviceFeatures RequiredFeatures = {};
	VkPhysicalDeviceVulkan11Features RequiredFeatures11 = {};
	VkPhysicalDeviceVulkan12Features RequiredFeatures12 = {};
	VkPhysicalDeviceVulkan13Features RequiredFeatures13 = {};
	FVulkanQueueRequirements QueueRequirements;
};
