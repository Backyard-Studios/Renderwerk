#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include "volk.h"
#include "VulkanAdapter.h"
#include "VulkanContext.h"

struct ENGINE_API FVulkanDeviceDesc
{
	TSharedPointer<FVulkanContext> Context;
	TSharedPointer<FVulkanAdapter> Adapter;
	VkSurfaceKHR Surface;
};

class ENGINE_API FVulkanDevice
{
public:
	FVulkanDevice(const FVulkanDeviceDesc& InDescription);
	~FVulkanDevice();

	DELETE_COPY_AND_MOVE(FVulkanDevice)

public:
	[[nodiscard]] FResult Initialize();
	void Destroy();

	[[nodiscard]] FResult WaitForIdle() const;

public:
	[[nodiscard]] VkDevice GetHandle() const { return Device; }

	[[nodiscard]] FVulkanQueueData GetQueueData() const { return QueueData; }
	[[nodiscard]] TSharedPointer<FVulkanAdapter> GetAdapter() const { return Description.Adapter; }

private:
	TVector<VkLayerProperties> AcquireAvailableLayers();
	TVector<VkExtensionProperties> AcquireAvailableExtensions();

	void ValidateLayers(const TVector<FVulkanRequireableComponent>& Layers, TVector<const char*>* OutValidatedLayers);
	void ValidateExtensions(const TVector<FVulkanRequireableComponent>& Extensions, TVector<const char*>* OutValidatedExtensions);

private:
	FVulkanDeviceDesc Description;

	FVulkanQueueData QueueData;
	VkDevice Device = VK_NULL_HANDLE;
};
