#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include <vulkan/vulkan.h>

#include "VulkanAdapter.h"

struct ENGINE_API FVulkanDeviceDesc
{
	VkAllocationCallbacks* Allocator = nullptr;
	TSharedPointer<FVulkanAdapter> Adapter;
	VkSurfaceKHR Surface = VK_NULL_HANDLE;
	FVulkanQueueRequirements QueueRequirements = {};
	TVector<FVulkanRequireableComponent> RequiredLayers;
	TVector<FVulkanRequireableComponent> RequiredExtensions;
	VkPhysicalDeviceFeatures EnabledFeatures = {};
	void* NextChain = nullptr;
};

class ENGINE_API FVulkanDevice
{
public:
	FVulkanDevice(const FVulkanDeviceDesc& InDescription);
	~FVulkanDevice();

	DELETE_COPY_AND_MOVE(FVulkanDevice)

public:
	[[nodiscard]] FResult Initialize();
	void Destroy() const;

	[[nodiscard]] FResult WaitForIdle() const;

public:
	[[nodiscard]] VkDevice GetHandle() const { return Device; }
	[[nodiscard]] TSharedPointer<FVulkanAdapter> GetAdapter() const { return Description.Adapter; }

	[[nodiscard]] TVector<const char*> GetActivatedLayers() const { return ActivatedLayers; }
	[[nodiscard]] TVector<const char*> GetActivatedExtensions() const { return ActivatedExtensions; }

	[[nodiscard]] FVulkanQueueData GetQueueData() const { return QueueData; }

	[[nodiscard]] VkQueue GetGraphicsQueue() const { return GraphicsQueue; }
	[[nodiscard]] VkQueue GetPresentQueue() const { return PresentQueue; }
	[[nodiscard]] VkQueue GetComputeQueue() const { return ComputeQueue; }
	[[nodiscard]] VkQueue GetTransferQueue() const { return TransferQueue; }

private:
	FVulkanDeviceDesc Description;

	TVector<const char*> ActivatedLayers;
	TVector<const char*> ActivatedExtensions;

	FVulkanQueueData QueueData = {};

	VkDevice Device = VK_NULL_HANDLE;

	VkQueue GraphicsQueue = VK_NULL_HANDLE;
	VkQueue PresentQueue = VK_NULL_HANDLE;
	VkQueue ComputeQueue = VK_NULL_HANDLE;
	VkQueue TransferQueue = VK_NULL_HANDLE;
};
