#include "pch.h"

#include "Renderwerk/Graphics/Renderer.h"

#include "Renderwerk/Graphics/VulkanUtils.h"

#if RW_PLATFORM_WINDOWS
#	include "vulkan/vulkan_win32.h"
#endif

FRenderer::FRenderer(const FRendererSettings& InSettings)
	: Settings(InSettings)
{
}

FRenderer::~FRenderer() = default;

FResult FRenderer::Initialize()
{
	FVulkanContextSettings VulkanContextSettings = {};
	VulkanContextSettings.APIVersion = VK_API_VERSION_1_3;
	VulkanContextSettings.bEnableValidation = Settings.bEnableValidation;
	VulkanContextSettings.bEnableDebugging = Settings.bEnableDebugging;
	VulkanContextSettings.RequestedExtensions.push_back({VK_KHR_SURFACE_EXTENSION_NAME, true});
#if RW_PLATFORM_WINDOWS
	VulkanContextSettings.RequestedExtensions.push_back({VK_KHR_WIN32_SURFACE_EXTENSION_NAME, true});
#endif
	VulkanContext = MakeShared<FVulkanContext>(VulkanContextSettings);
	RW_CHECK_RESULT(VulkanContext->Initialize());

#if RW_PLATFORM_WINDOWS
	VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo = {};
	SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	SurfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
	SurfaceCreateInfo.hwnd = static_cast<HWND>(Settings.Window->GetNativeHandle());
	RW_CHECK_VKRESULT(vkCreateWin32SurfaceKHR(VulkanContext->GetInstance(), &SurfaceCreateInfo, nullptr, &Surface), "Failed to create Win32 surface");
#endif

	FVulkanAdapterRequirements AdapterRequirements = {};
	AdapterRequirements.MinAPIVersion = VulkanContextSettings.APIVersion;
	AdapterRequirements.RequiredExtensions.push_back({VK_KHR_SWAPCHAIN_EXTENSION_NAME, true});
	AdapterRequirements.RequiredFeatures.samplerAnisotropy = true;
	AdapterRequirements.RequiredFeatures12.bufferDeviceAddress = true;
	AdapterRequirements.RequiredFeatures12.descriptorIndexing = true;
	AdapterRequirements.RequiredFeatures13.dynamicRendering = true;
	AdapterRequirements.RequiredFeatures13.synchronization2 = true;
	RW_CHECK_RESULT(SelectSuitableAdapter(VulkanContext, Surface, AdapterRequirements))

	RW_LOG_INFO("Selected adapter:");
	RW_LOG_INFO("\t- Name: {}", std::string(VulkanAdapter->GetDeviceProperties().deviceName));
	RW_LOG_INFO("\t- Type: {}", ToString(VulkanAdapter->GetDeviceProperties().deviceType));
	RW_LOG_INFO("\t- API Version: {}", VulkanFormatVersion(VulkanAdapter->GetDeviceProperties().apiVersion));
	RW_LOG_INFO("\t- Driver Version: {}", VulkanFormatVersion(VulkanAdapter->GetDeviceProperties().driverVersion));

	FVulkanDeviceDesc DeviceDesc = {};
	DeviceDesc.Adapter = VulkanAdapter;
	DeviceDesc.Surface = Surface;
	DeviceDesc.QueueRequirements = AdapterRequirements.QueueRequirements;
	DeviceDesc.RequiredLayers = AdapterRequirements.RequiredLayers;
	DeviceDesc.RequiredExtensions = AdapterRequirements.RequiredExtensions;
	DeviceDesc.EnabledFeatures = AdapterRequirements.RequiredFeatures;

	VkPhysicalDeviceBufferDeviceAddressFeatures BufferDeviceAddressFeatures = {};
	BufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	BufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;

	VkPhysicalDeviceDescriptorIndexingFeatures DescriptorIndexingFeatures = {};
	DescriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	DescriptorIndexingFeatures.pNext = &BufferDeviceAddressFeatures;

	VkPhysicalDeviceDynamicRenderingFeatures DynamicRenderingFeatures = {};
	DynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
	DynamicRenderingFeatures.pNext = &DescriptorIndexingFeatures;
	DynamicRenderingFeatures.dynamicRendering = VK_TRUE;

	VkPhysicalDeviceSynchronization2Features Synchronization2Features = {};
	Synchronization2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	Synchronization2Features.pNext = &DynamicRenderingFeatures;
	Synchronization2Features.synchronization2 = VK_TRUE;

	DeviceDesc.NextChain = &Synchronization2Features;

	VulkanDevice = MakeShared<FVulkanDevice>(DeviceDesc);
	RW_CHECK_RESULT(VulkanDevice->Initialize())

	FVulkanSwapchainDesc SwapchainDesc = {};
	SwapchainDesc.Allocator = nullptr;
	SwapchainDesc.Window = Settings.Window;
	SwapchainDesc.Surface = Surface;
	SwapchainDesc.Device = VulkanDevice;
	Swapchain = MakeShared<FVulkanSwapchain>(SwapchainDesc);
	RW_CHECK_RESULT(Swapchain->Initialize())

	MaxFrames = Swapchain->GetImageCount();
	Frames.resize(MaxFrames);
	for (FRenderFrame& Frame : Frames)
	{
		VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
		CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		CommandPoolCreateInfo.queueFamilyIndex = VulkanDevice->GetQueueData().GraphicsQueueIndex;
		RW_CHECK_VKRESULT(vkCreateCommandPool(VulkanDevice->GetHandle(), &CommandPoolCreateInfo, VulkanContextSettings.Allocator, &Frame.CommandPool),
		                  "Failed to create command pool");

		VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
		CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		CommandBufferAllocateInfo.commandPool = Frame.CommandPool;
		CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		CommandBufferAllocateInfo.commandBufferCount = 1;
		RW_CHECK_VKRESULT(vkAllocateCommandBuffers(VulkanDevice->GetHandle(), &CommandBufferAllocateInfo, &Frame.CommandBuffer), "Failed to allocate command buffer");

		VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
		SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		RW_CHECK_VKRESULT(vkCreateSemaphore(VulkanDevice->GetHandle(), &SemaphoreCreateInfo, VulkanContextSettings.Allocator, &Frame.ImageAvailableSemaphore),
		                  "Failed to create image available semaphore");
		RW_CHECK_VKRESULT(vkCreateSemaphore(VulkanDevice->GetHandle(), &SemaphoreCreateInfo, VulkanContextSettings.Allocator, &Frame.RenderFinishedSemaphore),
		                  "Failed to create render finished semaphore");

		VkFenceCreateInfo FenceCreateInfo = {};
		FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		RW_CHECK_VKRESULT(vkCreateFence(VulkanDevice->GetHandle(), &FenceCreateInfo, VulkanContextSettings.Allocator, &Frame.InFlightFence),
		                  "Failed to create in-flight fence");
	}

	return RW_RESULT_CODE_SUCCESS;
}

void FRenderer::Shutdown()
{
	if (VulkanDevice)
	{
		FResult Result = VulkanDevice->WaitForIdle();
		if (Result.IsError())
			RW_LOG_ERROR("Failed to wait for device to be idle");
	}

	for (FRenderFrame& Frame : Frames)
	{
		vkFreeCommandBuffers(VulkanDevice->GetHandle(), Frame.CommandPool, 1, &Frame.CommandBuffer);
		vkDestroyCommandPool(VulkanDevice->GetHandle(), Frame.CommandPool, nullptr);
		vkDestroySemaphore(VulkanDevice->GetHandle(), Frame.ImageAvailableSemaphore, nullptr);
		vkDestroySemaphore(VulkanDevice->GetHandle(), Frame.RenderFinishedSemaphore, nullptr);
		vkDestroyFence(VulkanDevice->GetHandle(), Frame.InFlightFence, nullptr);
	}
	Frames.clear();

	if (Swapchain)
		Swapchain->Destroy();
	Swapchain.Reset();
	if (VulkanDevice)
		VulkanDevice->Destroy();
	VulkanDevice.Reset();
	VulkanAdapter.Reset();
	vkDestroySurfaceKHR(VulkanContext->GetInstance(), Surface, nullptr);
	if (VulkanContext)
		VulkanContext->Destroy();
	VulkanContext.Reset();
}

FResult FRenderer::BeginFrame()
{
	RW_PROFILING_MARK_FUNCTION();

	FRenderFrame& Frame = Frames.at(FrameIndex);

	{
		RW_PROFILING_MARK_SCOPE("WaitForFence");
		RW_CHECK_VKRESULT(vkWaitForFences(VulkanDevice->GetHandle(), 1, &Frame.InFlightFence, VK_TRUE, 1000000000), "Failed to wait for in-flight fence")
		RW_CHECK_VKRESULT(vkResetFences(VulkanDevice->GetHandle(), 1, &Frame.InFlightFence), "Failed to reset in-flight fence")
	}

	{
		RW_PROFILING_MARK_SCOPE("AcquireNextImage");
		RW_CHECK_VKRESULT(
			vkAcquireNextImageKHR(VulkanDevice->GetHandle(), Swapchain->GetHandle(), 1000000000, Frame.ImageAvailableSemaphore, VK_NULL_HANDLE, &ImageIndex),
			"Failed to acquire next image")
	}

	VkCommandBuffer CommandBuffer = Frame.CommandBuffer;

	RW_CHECK_VKRESULT(vkResetCommandBuffer(CommandBuffer, 0), "Failed to reset command buffer")

	VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	RW_CHECK_VKRESULT(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo), "Failed to begin command buffer")

	{
		{
			VkImageMemoryBarrier2 ImageBarrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
			ImageBarrier.pNext = nullptr;

			ImageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
			ImageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
			ImageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
			ImageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

			ImageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			ImageBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;

			VkImageAspectFlags AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			VkImageSubresourceRange SubImage;
			SubImage.aspectMask = AspectMask;
			SubImage.baseMipLevel = 0;
			SubImage.levelCount = VK_REMAINING_MIP_LEVELS;
			SubImage.baseArrayLayer = 0;
			SubImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

			ImageBarrier.subresourceRange = SubImage;
			ImageBarrier.image = Swapchain->GetImage(ImageIndex);

			VkDependencyInfo DependencyInfo{};
			DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
			DependencyInfo.pNext = nullptr;

			DependencyInfo.imageMemoryBarrierCount = 1;
			DependencyInfo.pImageMemoryBarriers = &ImageBarrier;

			vkCmdPipelineBarrier2(CommandBuffer, &DependencyInfo);
		}

		VkClearColorValue ClearValue = {0.0f, 0.0f, 0.0f, 1.0f};
		VkImageSubresourceRange SubresourceRange;
		SubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		SubresourceRange.baseMipLevel = 0;
		SubresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
		SubresourceRange.baseArrayLayer = 0;
		SubresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
		vkCmdClearColorImage(CommandBuffer, Swapchain->GetImage(ImageIndex), VK_IMAGE_LAYOUT_GENERAL, &ClearValue, 1, &SubresourceRange);
	}

	return RW_RESULT_CODE_SUCCESS;
}

FResult FRenderer::EndFrame()
{
	RW_PROFILING_MARK_FUNCTION();

	FRenderFrame& Frame = Frames.at(FrameIndex);

	VkCommandBuffer CommandBuffer = Frame.CommandBuffer;

	{
		VkImageMemoryBarrier2 ImageBarrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
		ImageBarrier.pNext = nullptr;

		ImageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		ImageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
		ImageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		ImageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

		ImageBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
		ImageBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkImageAspectFlags AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		VkImageSubresourceRange SubImage;
		SubImage.aspectMask = AspectMask;
		SubImage.baseMipLevel = 0;
		SubImage.levelCount = VK_REMAINING_MIP_LEVELS;
		SubImage.baseArrayLayer = 0;
		SubImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

		ImageBarrier.subresourceRange = SubImage;
		ImageBarrier.image = Swapchain->GetImage(ImageIndex);

		VkDependencyInfo DependencyInfo{};
		DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		DependencyInfo.pNext = nullptr;

		DependencyInfo.imageMemoryBarrierCount = 1;
		DependencyInfo.pImageMemoryBarriers = &ImageBarrier;

		vkCmdPipelineBarrier2(CommandBuffer, &DependencyInfo);
	}

	RW_CHECK_VKRESULT(vkEndCommandBuffer(CommandBuffer), "Failed to end command buffer")

	VkCommandBufferSubmitInfo CommandBufferSubmitInfo = {};
	CommandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	CommandBufferSubmitInfo.commandBuffer = CommandBuffer;

	VkSemaphoreSubmitInfo ImageSemaphoreSubmitInfo = {};
	ImageSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	ImageSemaphoreSubmitInfo.semaphore = Frame.ImageAvailableSemaphore;
	ImageSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR;
	ImageSemaphoreSubmitInfo.value = 1;

	VkSemaphoreSubmitInfo RenderSemaphoreSubmitInfo = {};
	RenderSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	RenderSemaphoreSubmitInfo.semaphore = Frame.RenderFinishedSemaphore;
	RenderSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
	RenderSemaphoreSubmitInfo.value = 1;


	VkSubmitInfo2 SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	SubmitInfo.commandBufferInfoCount = 1;
	SubmitInfo.pCommandBufferInfos = &CommandBufferSubmitInfo;
	SubmitInfo.waitSemaphoreInfoCount = 1;
	SubmitInfo.pWaitSemaphoreInfos = &ImageSemaphoreSubmitInfo;
	SubmitInfo.signalSemaphoreInfoCount = 1;
	SubmitInfo.pSignalSemaphoreInfos = &RenderSemaphoreSubmitInfo;

	{
		RW_PROFILING_MARK_SCOPE("QueueSubmit");
		RW_CHECK_VKRESULT(vkQueueSubmit2(VulkanDevice->GetGraphicsQueue(), 1, &SubmitInfo, Frame.InFlightFence), "Failed to submit command buffer")
	}

	VkSwapchainKHR SwapchainHandle = Swapchain->GetHandle();
	VkPresentInfoKHR PresentInfo = {};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pWaitSemaphores = &Frame.RenderFinishedSemaphore;
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = &SwapchainHandle;
	PresentInfo.pImageIndices = &ImageIndex;

	{
		RW_PROFILING_MARK_SCOPE("Present");
		RW_CHECK_VKRESULT(vkQueuePresentKHR(VulkanDevice->GetPresentQueue(), &PresentInfo), "Failed to present image")
	}

	FrameIndex = (FrameIndex + 1) % MaxFrames;

	return RW_RESULT_CODE_SUCCESS;
}

FResult FRenderer::SelectSuitableAdapter(const TSharedPointer<FVulkanContext>& VulkanContext, const VkSurfaceKHR& Surface, const FVulkanAdapterRequirements& Requirements)
{
	uint32 DeviceCount = 0;
	vkEnumeratePhysicalDevices(VulkanContext->GetInstance(), &DeviceCount, nullptr);
	TVector<VkPhysicalDevice> PhysicalDevices(DeviceCount);
	vkEnumeratePhysicalDevices(VulkanContext->GetInstance(), &DeviceCount, PhysicalDevices.data());
	for (VkPhysicalDevice PhysicalDevice : PhysicalDevices)
	{
		VulkanAdapter.Reset();
		VulkanAdapter = MakeShared<FVulkanAdapter>(PhysicalDevice);
		RW_CHECK_RESULT(VulkanAdapter->Initialize())

		if (VulkanAdapter->GetDeviceProperties().apiVersion < Requirements.MinAPIVersion)
			continue;
		if (VulkanAdapter->GetDeviceProperties().deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && Requirements.bRequireDiscreteGPU)
			continue;
		if (!VulkanAdapter->SupportsLayers(Requirements.RequiredLayers))
			continue;
		if (!VulkanAdapter->SupportsExtensions(Requirements.RequiredExtensions))
			continue;
		if (!VulkanAdapter->SupportsFeatures(Requirements.RequiredFeatures))
			continue;
		if (!VulkanAdapter->SupportsFeatures11(Requirements.RequiredFeatures11))
			continue;
		if (!VulkanAdapter->SupportsFeatures12(Requirements.RequiredFeatures12))
			continue;
		if (!VulkanAdapter->SupportsFeatures13(Requirements.RequiredFeatures13))
			continue;

		FVulkanQueueData QueueData = {};
		RW_CHECK_RESULT(VulkanAdapter->QueryQueueData(Surface, Requirements.QueueRequirements, &QueueData))
		return RW_RESULT_CODE_SUCCESS;
	}
	VulkanAdapter.Reset();
	return FResult(RW_RESULT_CODE_FAIL, "Failed to find suitable Vulkan adapter");
}
