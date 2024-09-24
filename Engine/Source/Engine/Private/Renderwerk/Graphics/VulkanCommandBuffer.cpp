#include "pch.h"

#include "Renderwerk/Graphics/VulkanCommandBuffer.h"

#include "Renderwerk/Graphics/VulkanUtils.h"

FVulkanCommandPool::FVulkanCommandPool(const FVulkanCommandPoolDesc& InDescription)
	: Description(InDescription)
{
}

FVulkanCommandPool::~FVulkanCommandPool() = default;

FResult FVulkanCommandPool::Initialize()
{
	VkCommandPoolCreateInfo CommandPoolCreateInfo;
	CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CommandPoolCreateInfo.pNext = nullptr;
	CommandPoolCreateInfo.flags = Description.Flags;
	CommandPoolCreateInfo.queueFamilyIndex = Description.QueueIndex;

	CHECK_VKRESULT(vkCreateCommandPool(Description.Device->GetHandle(), &CommandPoolCreateInfo, Description.Context->GetAllocator(), &CommandPool),
	               "Failed to create command pool")

	return RESULT_SUCCESS;
}

void FVulkanCommandPool::Destroy()
{
	vkDestroyCommandPool(Description.Device->GetHandle(), CommandPool, Description.Context->GetAllocator());
}

FResult FVulkanCommandPool::Reset(const VkCommandPoolResetFlags Flags)
{
	CHECK_VKRESULT(vkResetCommandPool(Description.Device->GetHandle(), CommandPool, Flags), "Failed to reset command pool")
	return RESULT_SUCCESS;
}

FResult FVulkanCommandPool::AllocateCommandBuffer(TSharedPointer<FVulkanCommandBuffer>* OutCommandBuffer)
{
	VkCommandBufferAllocateInfo CommandBufferAllocateInfo;
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.pNext = nullptr;
	CommandBufferAllocateInfo.commandPool = CommandPool;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer CommandBuffer;
	CHECK_VKRESULT(vkAllocateCommandBuffers(Description.Device->GetHandle(), &CommandBufferAllocateInfo, &CommandBuffer), "Failed to allocate command buffer")

	FVulkanCommandBuffer* CommandBufferInstance = new FVulkanCommandBuffer(CommandBuffer);
	CommandBufferInstance->OwningCommandPool = this;
	OutCommandBuffer->Reset(CommandBufferInstance);
	return RESULT_SUCCESS;
}

void FVulkanCommandPool::FreeCommandBuffer(const TSharedPointer<FVulkanCommandBuffer>& CommandBuffer)
{
	RW_ASSERT(CommandBuffer->OwningCommandPool == this, RESULT_INVALID_ARGUMENT, "Command buffer does not belong to this command pool")
	vkFreeCommandBuffers(Description.Device->GetHandle(), CommandPool, 1, &CommandBuffer->CommandBuffer);
}

FVulkanCommandBuffer::FVulkanCommandBuffer(const VkCommandBuffer& InCommandBuffer)
	: CommandBuffer(InCommandBuffer)
{
}

FVulkanCommandBuffer::~FVulkanCommandBuffer() = default;

FResult FVulkanCommandBuffer::Reset(const VkCommandBufferResetFlags Flags) const
{
	CHECK_VKRESULT(vkResetCommandBuffer(CommandBuffer, Flags), "Failed to reset command buffer")
	return RESULT_SUCCESS;
}

FResult FVulkanCommandBuffer::Begin(const VkCommandBufferUsageFlags Flags) const
{
	VkCommandBufferBeginInfo CommandBufferBeginInfo;
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.pNext = nullptr;
	CommandBufferBeginInfo.flags = Flags;
	CommandBufferBeginInfo.pInheritanceInfo = nullptr;
	CHECK_VKRESULT(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo), "Failed to begin command buffer")
	return RESULT_SUCCESS;
}

FResult FVulkanCommandBuffer::End() const
{
	CHECK_VKRESULT(vkEndCommandBuffer(CommandBuffer), "Failed to end command buffer")
	return RESULT_SUCCESS;
}

void FVulkanCommandBuffer::TransitionSwapchainImage(const VkImage Image, const VkImageLayout OldLayout, const VkImageLayout NewLayout) const
{
	VkImageMemoryBarrier2 ImageBarrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
	ImageBarrier.pNext = nullptr;

	ImageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	ImageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	ImageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	ImageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

	ImageBarrier.oldLayout = OldLayout;
	ImageBarrier.newLayout = NewLayout;

	VkImageAspectFlags AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	VkImageSubresourceRange SubImage;
	SubImage.aspectMask = AspectMask;
	SubImage.baseMipLevel = 0;
	SubImage.levelCount = VK_REMAINING_MIP_LEVELS;
	SubImage.baseArrayLayer = 0;
	SubImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

	ImageBarrier.subresourceRange = SubImage;
	ImageBarrier.image = Image;

	VkDependencyInfo DependencyInfo{};
	DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	DependencyInfo.pNext = nullptr;

	DependencyInfo.imageMemoryBarrierCount = 1;
	DependencyInfo.pImageMemoryBarriers = &ImageBarrier;

	vkCmdPipelineBarrier2(CommandBuffer, &DependencyInfo);
}

void FVulkanCommandBuffer::ClearImage(const VkImage Image, const VkImageAspectFlags Flags, const VkClearColorValue ClearColor) const
{
	VkImageSubresourceRange SubresourceRange;
	SubresourceRange.aspectMask = Flags;
	SubresourceRange.baseMipLevel = 0;
	SubresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	SubresourceRange.baseArrayLayer = 0;
	SubresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
	vkCmdClearColorImage(CommandBuffer, Image, VK_IMAGE_LAYOUT_GENERAL, &ClearColor, 1, &SubresourceRange);
}
