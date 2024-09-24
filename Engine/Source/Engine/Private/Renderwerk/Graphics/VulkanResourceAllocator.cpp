#include "pch.h"

#include "Renderwerk/Graphics/VulkanResourceAllocator.h"

#include "Renderwerk/Graphics/VulkanUtils.h"
#include "Renderwerk/Renderer/Renderer.h"

FVulkanResourceAllocator::FVulkanResourceAllocator() = default;

FVulkanResourceAllocator::~FVulkanResourceAllocator() = default;

FResult FVulkanResourceAllocator::Initialize()
{
	VmaVulkanFunctions VulkanFunctions = {};
	VulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
	VulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo AllocatorCreateInfo = {};
	AllocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT | VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
	AllocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
	AllocatorCreateInfo.pAllocationCallbacks = GetRenderer()->GetVulkanContext()->GetAllocator();
	AllocatorCreateInfo.instance = GetRenderer()->GetVulkanContext()->GetInstance();
	AllocatorCreateInfo.physicalDevice = GetRenderer()->GetAdapter()->GetHandle();
	AllocatorCreateInfo.device = GetRenderer()->GetDevice()->GetHandle();
	AllocatorCreateInfo.pVulkanFunctions = &VulkanFunctions;

	CHECK_VKRESULT(vmaCreateAllocator(&AllocatorCreateInfo, &ResourceAllocator), "Failed to create Vulkan resource allocator");

	return RESULT_SUCCESS;
}

void FVulkanResourceAllocator::Destroy() const
{
	vmaDestroyAllocator(ResourceAllocator);
}

FResult FVulkanResourceAllocator::AllocateBuffer(const VkBufferCreateInfo& BufferCreateInfo, const VmaMemoryUsage MemoryUsage,
                                                 FVulkanAllocatedBuffer& OutAllocatedBuffer) const
{
	VmaAllocationCreateInfo AllocationCreateInfo = {};
	AllocationCreateInfo.usage = MemoryUsage;
	CHECK_VKRESULT(
		vmaCreateBuffer(ResourceAllocator, &BufferCreateInfo, &AllocationCreateInfo, &OutAllocatedBuffer.Buffer, &OutAllocatedBuffer.Allocation, &OutAllocatedBuffer.
			AllocationInfo), "Failed to allocate buffer")
	return RESULT_SUCCESS;
}

void FVulkanResourceAllocator::FreeBuffer(const FVulkanAllocatedBuffer& AllocatedBuffer) const
{
	vmaDestroyBuffer(ResourceAllocator, AllocatedBuffer.Buffer, AllocatedBuffer.Allocation);
}

FResult FVulkanResourceAllocator::AllocateImage(const VkImageCreateInfo& ImageCreateInfo, const VmaMemoryUsage MemoryUsage,
                                                FVulkanAllocatedImage& OutAllocatedImage) const
{
	VmaAllocationCreateInfo AllocationCreateInfo = {};
	AllocationCreateInfo.usage = MemoryUsage;
	CHECK_VKRESULT(
		vmaCreateImage(ResourceAllocator, &ImageCreateInfo, &AllocationCreateInfo, &OutAllocatedImage.Image, &OutAllocatedImage.Allocation, &OutAllocatedImage.
			AllocationInfo), "Failed to allocate image")
	return RESULT_SUCCESS;
}

void FVulkanResourceAllocator::FreeImage(const FVulkanAllocatedImage& AllocatedImage) const
{
	vmaDestroyImage(ResourceAllocator, AllocatedImage.Image, AllocatedImage.Allocation);
}

void* FVulkanResourceAllocator::MapMemory(const VmaAllocation Allocation) const
{
	void* Data = nullptr;
	vmaMapMemory(ResourceAllocator, Allocation, &Data);
	return Data;
}

void FVulkanResourceAllocator::UnmapMemory(const VmaAllocation Allocation) const
{
	vmaUnmapMemory(ResourceAllocator, Allocation);
}
