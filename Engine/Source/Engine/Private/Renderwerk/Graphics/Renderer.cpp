﻿#include "pch.h"

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
		RW_CHECK_RESULT(VulkanAdapter->Initialize(), "Failed to initialize Vulkan adapter")

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
		RW_CHECK_RESULT(VulkanAdapter->QueryQueueData(Surface, Requirements.QueueRequirements, &QueueData), "Failed to query queue data for Vulkan adapter")
		return RW_RESULT_CODE_SUCCESS;
	}
	VulkanAdapter.Reset();
	return FResult(RW_RESULT_CODE_FAIL, "Failed to find suitable Vulkan adapter");
}
