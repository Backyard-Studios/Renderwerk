#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include <vulkan/vulkan.h>

#include "VulkanDevice.h"

#include "Renderwerk/Platform/Window.h"

struct ENGINE_API FVulkanSwapchainDesc
{
	VkAllocationCallbacks* Allocator = nullptr;
	TSharedPointer<IWindow> Window;
	VkSurfaceKHR Surface = VK_NULL_HANDLE;
	TSharedPointer<FVulkanDevice> Device;
	VkSurfaceFormatKHR Format = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
	uint32 BufferCount = 3;
};

class ENGINE_API FVulkanSwapchain
{
public:
	FVulkanSwapchain(const FVulkanSwapchainDesc& InDescription);
	~FVulkanSwapchain();

	DELETE_COPY_AND_MOVE(FVulkanSwapchain)

public:
	[[nodiscard]] FResult Initialize();
	void Destroy();

public:
	[[nodiscard]] VkSwapchainKHR GetHandle() const { return Swapchain; }

private:
	void ChooseSurfaceFormat(const FVulkanSurfaceProperties& SurfaceProperties);
	void ChoosePresentMode(const FVulkanSurfaceProperties& SurfaceProperties);

	FResult CreateSwapchain(VkSwapchainKHR OldSwapchain, VkSwapchainKHR* OutSwapchain);
	FResult AcquireImages();

	void DestroyImages();

private:
	FVulkanSwapchainDesc Description;

	VkSurfaceFormatKHR SurfaceFormat = {};
	VkPresentModeKHR PresentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;

	VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
	VkExtent2D Extent = {0, 0};

	TVector<VkImage> Images;
	TVector<VkImageView> ImageViews;
};
