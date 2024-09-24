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

	// Weird naming, but `CreateSemaphore` is a stupid Windows macro
	[[nodiscard]] FResult CreateVulkanSemaphore(const VkSemaphoreCreateFlags& Flags, VkSemaphore* OutSemaphore);
	[[nodiscard]] FResult CreateFence(const VkFenceCreateFlags& Flags, VkFence* OutFence);

	[[nodiscard]] FResult WaitForFence(const VkFence& Fence, bool8 bShouldReset = true, uint64 Timeout = 1000000000) const;

public:
	[[nodiscard]] VkDevice GetHandle() const { return Device; }

	[[nodiscard]] FVulkanQueueData GetQueueData() const { return QueueData; }
	[[nodiscard]] TSharedPointer<FVulkanAdapter> GetAdapter() const { return Description.Adapter; }

	[[nodiscard]] VkQueue GetGraphicsQueue() const { return GraphicsQueue; }
	[[nodiscard]] VkQueue GetPresentQueue() const { return PresentQueue; }
	[[nodiscard]] VkQueue GetTransferQueue() const { return TransferQueue; }
	[[nodiscard]] VkQueue GetComputeQueue() const { return ComputeQueue; }

private:
	TVector<VkLayerProperties> AcquireAvailableLayers();
	TVector<VkExtensionProperties> AcquireAvailableExtensions();

	void ValidateLayers(const TVector<FVulkanRequireableComponent>& Layers, TVector<const char*>* OutValidatedLayers);
	void ValidateExtensions(const TVector<FVulkanRequireableComponent>& Extensions, TVector<const char*>* OutValidatedExtensions);

private:
	FVulkanDeviceDesc Description;

	FVulkanQueueData QueueData;
	VkDevice Device = VK_NULL_HANDLE;

	VkQueue GraphicsQueue = VK_NULL_HANDLE;
	VkQueue PresentQueue = VK_NULL_HANDLE;
	VkQueue TransferQueue = VK_NULL_HANDLE;
	VkQueue ComputeQueue = VK_NULL_HANDLE;
};
