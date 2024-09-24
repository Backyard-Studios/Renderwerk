#pragma once

#include "VulkanAdapter.h"
#include "VulkanDevice.h"

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Platform/Window.h"

#include "volk.h"

struct FRenderFrameData;

struct ENGINE_API FVulkanSwapchainDesc
{
	TSharedPointer<FVulkanContext> Context;
	TSharedPointer<FVulkanDevice> Device;
	TSharedPointer<FWindow> Window;
	VkSurfaceKHR Surface;
	FVulkanSurfaceProperties SurfaceProperties;
	uint32 ImageCount = 3;
	VkSurfaceFormatKHR PreferredFormat = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
	VkPresentModeKHR PreferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
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

	[[nodiscard]] FResult Resize();

	[[nodiscard]] FResult AcquireNextImageIndex(VkSemaphore Semaphore, uint32* OutImageIndex);
	[[nodiscard]] FResult Present(const FRenderFrameData& RenderFrame);

	[[nodiscard]] VkImage GetImage(uint32 Index) const;
	[[nodiscard]] VkImageView GetImageView(uint32 Index) const;

public:
	[[nodiscard]] VkSwapchainKHR GetHandle() const { return Swapchain; }

	[[nodiscard]] VkSurfaceFormatKHR GetSurfaceFormat() const { return SurfaceFormat; }
	[[nodiscard]] VkPresentModeKHR GetPresentMode() const { return PresentMode; }
	[[nodiscard]] uint32 GetImageCount() const { return ImageCount; }
	[[nodiscard]] VkExtent2D GetExtent() const { return Extent; }

private:
	[[nodiscard]] FResult CreateSwapchain();

	void ChooseSurfaceFormat();
	void ChoosePresentMode();

	[[nodiscard]] FResult AcquireImages();

	void ReleaseImages();
	void DestroySwapchain();

private:
	FVulkanSwapchainDesc Description;

	VkSurfaceFormatKHR SurfaceFormat = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
	VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR;

	uint32 ImageCount = 0;
	VkExtent2D Extent = {0, 0};

	VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
	TVector<VkImage> Images;
	TVector<VkImageView> ImageViews;
};
