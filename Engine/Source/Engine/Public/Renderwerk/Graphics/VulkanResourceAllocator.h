#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include "volk.h"

#include <vk_mem_alloc.h>

struct ENGINE_API FVulkanAllocatedBuffer
{
	VkBuffer Buffer = VK_NULL_HANDLE;
	VmaAllocation Allocation = VK_NULL_HANDLE;
	VmaAllocationInfo AllocationInfo = {};
};

struct ENGINE_API FVulkanAllocatedImage
{
	VkImage Image = VK_NULL_HANDLE;
	VmaAllocation Allocation = VK_NULL_HANDLE;
	VmaAllocationInfo AllocationInfo = {};
};

class ENGINE_API FVulkanResourceAllocator
{
public:
	FVulkanResourceAllocator();
	~FVulkanResourceAllocator();

	DELETE_COPY_AND_MOVE(FVulkanResourceAllocator)

public:
	[[nodiscard]] FResult Initialize();
	void Destroy() const;

	[[nodiscard]] FResult AllocateBuffer(const VkBufferCreateInfo& BufferCreateInfo, VmaMemoryUsage MemoryUsage, FVulkanAllocatedBuffer& OutAllocatedBuffer) const;
	void FreeBuffer(const FVulkanAllocatedBuffer& AllocatedBuffer) const;

	[[nodiscard]] FResult AllocateImage(const VkImageCreateInfo& ImageCreateInfo, VmaMemoryUsage MemoryUsage, FVulkanAllocatedImage& OutAllocatedImage) const;
	void FreeImage(const FVulkanAllocatedImage& AllocatedImage) const;

	void* MapMemory(VmaAllocation Allocation) const;
	void UnmapMemory(VmaAllocation Allocation) const;

	template <typename T>
	T* MapMemory(const VmaAllocation Allocation) const
	{
		return static_cast<T*>(MapMemory(Allocation));
	}

public:
	[[nodiscard]] VmaAllocator GetHandle() const { return ResourceAllocator; }

private:
	VmaAllocator ResourceAllocator = VK_NULL_HANDLE;
};
