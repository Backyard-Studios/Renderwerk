#include "pch.h"

#include "Renderwerk/Graphics/VulkanSwapchain.h"

#include "Renderwerk/Graphics/VulkanUtils.h"

FVulkanSwapchain::FVulkanSwapchain(const FVulkanSwapchainDesc& InDescription)
	: Description(InDescription)
{
}

FVulkanSwapchain::~FVulkanSwapchain() = default;

FResult FVulkanSwapchain::Initialize()
{
	ChooseSurfaceFormat();
	ChoosePresentMode();
	CHECK_RESULT(CreateSwapchain())
	CHECK_RESULT(AcquireImages())
	RW_LOG_INFO("Created swapchain for window '{}' [Guid: {},  Size: {}x{}]:", Description.Window->GetWindowState().Title, Description.Window->GetGuid().ToString(),
	            Description.Window->GetWindowState().ClientWidth, Description.Window->GetWindowState().ClientHeight);
	RW_LOG_INFO("\t- Image count: {}", ImageCount);
	RW_LOG_INFO("\t- Format: {}", ToString(SurfaceFormat.format));
	RW_LOG_INFO("\t- Color space: {}", ToString(SurfaceFormat.colorSpace));
	RW_LOG_INFO("\t- Present mode: {}", ToString(PresentMode));
	return RESULT_SUCCESS;
}

void FVulkanSwapchain::Destroy()
{
	ReleaseImages();
	DestroySwapchain();
}

FResult FVulkanSwapchain::Resize()
{
	ReleaseImages();
	DestroySwapchain();
	CHECK_RESULT(CreateSwapchain())
	CHECK_RESULT(AcquireImages())
	return RESULT_SUCCESS;
}

FResult FVulkanSwapchain::AcquireNextImageIndex(const VkSemaphore Semaphore, uint32* OutImageIndex)
{
	VkResult Result = vkAcquireNextImageKHR(Description.Device->GetHandle(), Swapchain, 1000000000, Semaphore, VK_NULL_HANDLE, OutImageIndex);
	if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
		return RESULT_VULKAN_SWAPCHAIN_OUT_OF_DATE;
	CHECK_VKRESULT(Result, "Failed to acquire next image")
	return RESULT_SUCCESS;
}

VkImageView FVulkanSwapchain::GetImageView(const uint32 Index) const
{
	RW_ASSERT(Index < ImageViews.size(), RESULT_FAILED, "Index out of range")
	return ImageViews.at(Index);
}

FResult FVulkanSwapchain::CreateSwapchain()
{
	// Query surface capabilities, because we need the current extent if this is called after a window resize
	VkSurfaceCapabilitiesKHR SurfaceCapabilities;
	CHECK_RESULT(Description.Device->GetAdapter()->QuerySurfaceCapabilities(Description.Surface, &SurfaceCapabilities))

	VkSwapchainCreateInfoKHR SwapchainCreateInfo;
	SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	SwapchainCreateInfo.pNext = nullptr;
	SwapchainCreateInfo.flags = 0;
	SwapchainCreateInfo.surface = Description.Surface;

	ImageCount = Description.ImageCount;
	if (Description.ImageCount < SurfaceCapabilities.minImageCount)
	{
		RW_LOG_WARN("Requested image count is less than the minimum image count. Using the minimum image count (Min: {}) instead.", SurfaceCapabilities.minImageCount);
		ImageCount = SurfaceCapabilities.minImageCount;
	}
	else if (Description.ImageCount > SurfaceCapabilities.maxImageCount)
	{
		RW_LOG_WARN("Requested image count is greater than the maximum image count. Using the maximum image count (Max: {}) instead.", SurfaceCapabilities.maxImageCount);
		ImageCount = SurfaceCapabilities.maxImageCount;
	}
	SwapchainCreateInfo.minImageCount = ImageCount;

	SwapchainCreateInfo.imageFormat = SurfaceFormat.format;
	SwapchainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
	SwapchainCreateInfo.imageExtent = SurfaceCapabilities.currentExtent;
	SwapchainCreateInfo.imageArrayLayers = 1;
	SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	FVulkanQueueData QueueData = Description.Device->GetQueueData();
	uint32 QueueIndices[] = {QueueData.GraphicsIndex, QueueData.PresentIndex};

	if (QueueData.GraphicsIndex != QueueData.PresentIndex)
	{
		SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		SwapchainCreateInfo.queueFamilyIndexCount = _countof(QueueIndices);
		SwapchainCreateInfo.pQueueFamilyIndices = QueueIndices;
	}
	else
	{
		SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		SwapchainCreateInfo.queueFamilyIndexCount = 0;
		SwapchainCreateInfo.pQueueFamilyIndices = nullptr;
	}

	SwapchainCreateInfo.preTransform = SurfaceCapabilities.currentTransform;
	SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	SwapchainCreateInfo.presentMode = PresentMode;
	SwapchainCreateInfo.clipped = VK_TRUE;
	SwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	Extent = SwapchainCreateInfo.imageExtent;

	VkResult CreateResult = vkCreateSwapchainKHR(Description.Device->GetHandle(), &SwapchainCreateInfo, Description.Context->GetAllocator(), &Swapchain);
	CHECK_VKRESULT(CreateResult, "Failed to create Vulkan swapchain")

	return RESULT_SUCCESS;
}

void FVulkanSwapchain::ChooseSurfaceFormat()
{
	for (VkSurfaceFormatKHR AvailableFormat : Description.SurfaceProperties.Formats)
	{
		if (AvailableFormat.format == Description.PreferredFormat.format && AvailableFormat.colorSpace == Description.PreferredFormat.colorSpace)
		{
			SurfaceFormat = AvailableFormat;
			return;
		}
	}
	RW_LOG_WARN("Failed to find preferred surface format. Using the first available format instead.");
	SurfaceFormat = Description.SurfaceProperties.Formats.at(0);
}

void FVulkanSwapchain::ChoosePresentMode()
{
	for (VkPresentModeKHR AvailablePresentMode : Description.SurfaceProperties.PresentModes)
	{
		if (AvailablePresentMode == Description.PreferredPresentMode)
		{
			PresentMode = AvailablePresentMode;
			return;
		}
	}
	RW_LOG_WARN("Failed to find preferred present mode. Using FIFO instead.");
	PresentMode = VK_PRESENT_MODE_FIFO_KHR;
}

FResult FVulkanSwapchain::AcquireImages()
{
	Images.resize(ImageCount);
	ImageViews.resize(ImageCount);
	CHECK_VKRESULT(vkGetSwapchainImagesKHR(Description.Device->GetHandle(), Swapchain, &ImageCount, Images.data()), "Failed to get swapchain images")
	for (VkImage Image : Images)
	{
		VkImageViewCreateInfo ImageViewCreateInfo = {};
		ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageViewCreateInfo.pNext = nullptr;
		ImageViewCreateInfo.flags = 0;
		ImageViewCreateInfo.image = Image;
		ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageViewCreateInfo.format = SurfaceFormat.format;
		ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ImageViewCreateInfo.subresourceRange.levelCount = 1;
		ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ImageViewCreateInfo.subresourceRange.layerCount = 1;

		VkImageView ImageView = VK_NULL_HANDLE;
		CHECK_VKRESULT(vkCreateImageView(Description.Device->GetHandle(), &ImageViewCreateInfo, Description.Context->GetAllocator(), &ImageView),
		               "Failed to create image view");
		ImageViews.push_back(ImageView);
	}
	return RESULT_SUCCESS;
}

void FVulkanSwapchain::ReleaseImages()
{
	for (VkImageView ImageView : ImageViews)
		vkDestroyImageView(Description.Device->GetHandle(), ImageView, Description.Context->GetAllocator());
	ImageViews.clear();
	Images.clear();
}

void FVulkanSwapchain::DestroySwapchain()
{
	vkDestroySwapchainKHR(Description.Device->GetHandle(), Swapchain, Description.Context->GetAllocator());
}
