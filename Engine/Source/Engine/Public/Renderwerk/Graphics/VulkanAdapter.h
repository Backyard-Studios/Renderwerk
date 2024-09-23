#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"

#include "volk.h"

// https://pcisig.com/membership/member-companies
enum class ENGINE_API EVulkanVendor : uint32
{
	Unknown = 0,
	AMD = 0x1022,
	NVIDIA = 0x10DE,
	Intel = 0x8086,
};

ENGINE_API std::string ToString(const EVulkanVendor& Vendor);

struct ENGINE_API FVulkanSurfaceProperties
{
	VkSurfaceCapabilitiesKHR Capabilities;
	TVector<VkSurfaceFormatKHR> Formats;
	TVector<VkPresentModeKHR> PresentModes;
};

struct ENGINE_API FVulkanQueueData
{
	uint32 GraphicsIndex = UINT32_MAX;
	uint32 PresentIndex = UINT32_MAX;
	uint32 TransferIndex = UINT32_MAX;
	uint32 ComputeIndex = UINT32_MAX;

	[[nodiscard]] bool8 IsValid() const
	{
		return GraphicsIndex != UINT32_MAX && PresentIndex != UINT32_MAX && TransferIndex != UINT32_MAX && ComputeIndex != UINT32_MAX;
	}
};

class ENGINE_API FVulkanAdapter
{
public:
	FVulkanAdapter(const VkPhysicalDevice& InPhysicalDevice, uint32 InIndex);
	~FVulkanAdapter();

	DELETE_COPY_AND_MOVE(FVulkanAdapter)

public:
	[[nodiscard]] FResult Initialize();
	// No need for `Destroy` method

	[[nodiscard]] bool8 IsDiscrete() const;
	[[nodiscard]] EVulkanVendor GetVendor() const;
	[[nodiscard]] FResult QuerySurfaceCapabilities(const VkSurfaceKHR& Surface, VkSurfaceCapabilitiesKHR* OutCapabilities) const;
	[[nodiscard]] FResult QuerySurfaceProperties(const VkSurfaceKHR& Surface, FVulkanSurfaceProperties* OutProperties) const;
	[[nodiscard]] FResult QueryQueueData(const VkSurfaceKHR& Surface, FVulkanQueueData* OutQueueData) const;

public:
	[[nodiscard]] VkPhysicalDevice GetHandle() const { return PhysicalDevice; }

	[[nodiscard]] VkPhysicalDeviceProperties GetProperties() const { return Properties; }
	[[nodiscard]] VkPhysicalDeviceLimits GetLimits() const { return Properties.limits; }
	[[nodiscard]] VkPhysicalDeviceMemoryProperties GetMemoryProperties() const { return MemoryProperties; }

	[[nodiscard]] VkPhysicalDeviceFeatures GetFeatures() const { return Features; }
	[[nodiscard]] VkPhysicalDeviceVulkan11Features GetCore11Features() const { return Core11Features; }
	[[nodiscard]] VkPhysicalDeviceVulkan12Features GetCore12Features() const { return Core12Features; }
	[[nodiscard]] VkPhysicalDeviceVulkan13Features GetCore13Features() const { return Core13Features; }

private:
	VkPhysicalDevice PhysicalDevice;
	uint32 Index;

	VkPhysicalDeviceProperties Properties = {};
	VkPhysicalDeviceMemoryProperties MemoryProperties = {};

	VkPhysicalDeviceFeatures Features = {};
	VkPhysicalDeviceVulkan11Features Core11Features = {};
	VkPhysicalDeviceVulkan12Features Core12Features = {};
	VkPhysicalDeviceVulkan13Features Core13Features = {};
};
