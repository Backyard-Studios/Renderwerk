#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include "volk.h"
#include "VulkanDevice.h"

class FVulkanCommandBuffer;

struct ENGINE_API FVulkanCommandPoolDesc
{
	TSharedPointer<FVulkanContext> Context;
	TSharedPointer<FVulkanDevice> Device;
	uint32 QueueIndex = 0;
	VkCommandPoolCreateFlags Flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
};

class ENGINE_API FVulkanCommandPool
{
public:
	FVulkanCommandPool(const FVulkanCommandPoolDesc& InDescription);
	~FVulkanCommandPool();

	DELETE_COPY_AND_MOVE(FVulkanCommandPool)

public:
	[[nodiscard]] FResult Initialize();
	void Destroy();

	[[nodiscard]] FResult Reset(VkCommandPoolResetFlags Flags = 0);

	[[nodiscard]] FResult AllocateCommandBuffer(TSharedPointer<FVulkanCommandBuffer>* OutCommandBuffer);
	void FreeCommandBuffer(const TSharedPointer<FVulkanCommandBuffer>& CommandBuffer);

public:
	[[nodiscard]] VkCommandPool GetHandle() const { return CommandPool; }

private:
	FVulkanCommandPoolDesc Description;

	VkCommandPool CommandPool = VK_NULL_HANDLE;
};

class ENGINE_API FVulkanCommandBuffer
{
public:
	FVulkanCommandBuffer(const VkCommandBuffer& InCommandBuffer);
	~FVulkanCommandBuffer();

	DELETE_COPY_AND_MOVE(FVulkanCommandBuffer)

public:
	[[nodiscard]] FResult Reset(VkCommandBufferResetFlags Flags = 0) const;

	[[nodiscard]] FResult Begin(VkCommandBufferUsageFlags Flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) const;
	[[nodiscard]] FResult End() const;

	void TransitionSwapchainImage(VkImage Image, VkImageLayout OldLayout, VkImageLayout NewLayout) const;
	void ClearImage(VkImage Image, VkImageAspectFlags Flags = VK_IMAGE_ASPECT_COLOR_BIT, VkClearColorValue ClearColor = DefaultClearColor) const;

public:
	[[nodiscard]] VkCommandBuffer GetHandle() const { return CommandBuffer; }

private:
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	static constexpr VkClearColorValue DefaultClearColor = {0.0f, 0.0f, 0.0f, 1.0f};

	// Only used for debugging purposes.
	FVulkanCommandPool* OwningCommandPool = nullptr;

	friend class FVulkanCommandPool;
};
