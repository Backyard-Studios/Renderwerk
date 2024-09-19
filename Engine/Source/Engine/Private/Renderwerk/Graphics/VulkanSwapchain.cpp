#include "pch.h"

#include "Renderwerk/Graphics/VulkanSwapchain.h"

#include "Renderwerk/Graphics/Renderer.h"
#include "Renderwerk/Graphics/VulkanUtils.h"

FVulkanSwapchain::FVulkanSwapchain(const FVulkanSwapchainDesc& InDescription)
	: Description(InDescription)
{
}

FVulkanSwapchain::~FVulkanSwapchain()
{
}

FResult FVulkanSwapchain::Initialize()
{
	FVulkanSurfaceProperties SurfaceProperties;
	RW_CHECK_RESULT(Description.Device->GetAdapter()->QuerySurfaceProperties(Description.Surface, &SurfaceProperties));
	ChooseSurfaceFormat(SurfaceProperties);
	ChoosePresentMode(SurfaceProperties);
	RW_LOG_DEBUG("Surface format: {} {}", ToString(SurfaceFormat.format), ToString(SurfaceFormat.colorSpace));
	RW_LOG_DEBUG("Present mode: {}", ToString(PresentMode));
	RW_CHECK_RESULT(CreateSwapchain(VK_NULL_HANDLE, &Swapchain));
	RW_CHECK_RESULT(AcquireImages());
	return RW_RESULT_CODE_SUCCESS;
}

void FVulkanSwapchain::Destroy()
{
	FResult Result = Description.Device->WaitForIdle();
	if (Result.IsError())
		RW_LOG_ERROR("Failed to wait for device to be idle");
	DestroyImages();
	vkDestroySwapchainKHR(Description.Device->GetHandle(), Swapchain, Description.Allocator);
}

void FVulkanSwapchain::ChooseSurfaceFormat(const FVulkanSurfaceProperties& SurfaceProperties)
{
	for (VkSurfaceFormatKHR AvailableFormat : SurfaceProperties.Formats)
	{
		if (AvailableFormat.format == Description.Format.format && AvailableFormat.colorSpace == Description.Format.colorSpace)
		{
			SurfaceFormat = AvailableFormat;
			return;
		}
	}
	SurfaceFormat = SurfaceProperties.Formats.at(0);
}

void FVulkanSwapchain::ChoosePresentMode(const FVulkanSurfaceProperties& SurfaceProperties)
{
	for (VkPresentModeKHR AvailablePresentMode : SurfaceProperties.PresentModes)
	{
		if (AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			PresentMode = AvailablePresentMode;
			return;
		}
	}
	PresentMode = VK_PRESENT_MODE_FIFO_KHR;
}

FResult FVulkanSwapchain::CreateSwapchain(const VkSwapchainKHR OldSwapchain, VkSwapchainKHR* OutSwapchain)
{
	FVulkanSurfaceProperties SurfaceProperties;
	RW_CHECK_RESULT(Description.Device->GetAdapter()->QuerySurfaceProperties(Description.Surface, &SurfaceProperties));

	VkSwapchainCreateInfoKHR SwapchainCreateInfo;
	SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	SwapchainCreateInfo.pNext = nullptr;
	SwapchainCreateInfo.flags = 0;
	SwapchainCreateInfo.surface = Description.Surface;
	SwapchainCreateInfo.minImageCount = std::max(Description.BufferCount, SurfaceProperties.Capabilities.minImageCount);
	if (SwapchainCreateInfo.minImageCount > SurfaceProperties.Capabilities.maxImageCount)
	{
		RW_LOG_WARN("Requested buffer count exceeds maximum image count. Using {} instead", SurfaceProperties.Capabilities.maxImageCount);
		SwapchainCreateInfo.minImageCount = SurfaceProperties.Capabilities.maxImageCount;
	}
	SwapchainCreateInfo.imageFormat = SurfaceFormat.format;
	SwapchainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
	SwapchainCreateInfo.imageExtent = SurfaceProperties.Capabilities.currentExtent;
	SwapchainCreateInfo.imageArrayLayers = 1;
	SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	FVulkanQueueData QueueData = Description.Device->GetQueueData();
	uint32 QueueIndices[] = {QueueData.GraphicsQueueIndex, QueueData.PresentQueueIndex};

	if (!QueueData.IsGraphicsSharingWithPresent())
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

	SwapchainCreateInfo.preTransform = SurfaceProperties.Capabilities.currentTransform;
	SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	SwapchainCreateInfo.presentMode = PresentMode;
	SwapchainCreateInfo.clipped = VK_TRUE;
	SwapchainCreateInfo.oldSwapchain = OldSwapchain;

	RW_CHECK_VKRESULT(vkCreateSwapchainKHR(Description.Device->GetHandle(), &SwapchainCreateInfo, Description.Allocator, OutSwapchain), "Failed to create swapchain");

	Extent = SwapchainCreateInfo.imageExtent;

	return RW_RESULT_CODE_SUCCESS;
}

FResult FVulkanSwapchain::AcquireImages()
{
	uint32 ImageCount = 0;
	RW_CHECK_VKRESULT(vkGetSwapchainImagesKHR(Description.Device->GetHandle(), Swapchain, &ImageCount, nullptr), "Failed to get swapchain images");
	Images.resize(ImageCount);
	RW_CHECK_VKRESULT(vkGetSwapchainImagesKHR(Description.Device->GetHandle(), Swapchain, &ImageCount, Images.data()), "Failed to get swapchain images");

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
		RW_CHECK_VKRESULT(vkCreateImageView(Description.Device->GetHandle(), &ImageViewCreateInfo, Description.Allocator, &ImageView), "Failed to create image view");
		ImageViews.push_back(ImageView);
	}

	return RW_RESULT_CODE_SUCCESS;
}

void FVulkanSwapchain::DestroyImages()
{
	for (VkImageView ImageView : ImageViews)
		vkDestroyImageView(Description.Device->GetHandle(), ImageView, Description.Allocator);
	ImageViews.clear();
	Images.clear();
}
