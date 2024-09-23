#include "pch.h"

#include "Renderwerk/Graphics/VulkanDevice.h"

#include "Renderwerk/Graphics/VulkanUtils.h"

FVulkanDevice::FVulkanDevice(const FVulkanDeviceDesc& InDescription)
	: Description(InDescription)
{
}

FVulkanDevice::~FVulkanDevice()
{
}

FResult FVulkanDevice::Initialize()
{
	CHECK_RESULT(Description.Adapter->QueryQueueData(Description.Surface, &QueueData));

	TSet<uint32> Indices = {QueueData.GraphicsIndex, QueueData.PresentIndex, QueueData.TransferIndex, QueueData.ComputeIndex};
	TVector<VkDeviceQueueCreateInfo> QueueCreateInfos;

	constexpr float32 QueuePriorities[] = {0.9f, 1.0f};
	for (uint32 Index : Indices)
	{
		VkDeviceQueueCreateInfo QueueCreateInfo = {};
		QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		QueueCreateInfo.pNext = nullptr;
		QueueCreateInfo.flags = 0;
		QueueCreateInfo.queueFamilyIndex = Index;

		if (Index == QueueData.GraphicsIndex && Index == QueueData.PresentIndex)
		{
			QueueCreateInfo.queueCount = 2;
		}
		else
		{
			QueueCreateInfo.queueCount = 1;
		}

		QueueCreateInfo.pQueuePriorities = QueuePriorities;
		QueueCreateInfos.push_back(QueueCreateInfo);
	}

	TVector<FVulkanRequireableComponent> Layers = {};
	TVector<const char*> ValidatedLayers;
	ValidateLayers(Layers, &ValidatedLayers);

	TVector<FVulkanRequireableComponent> Extensions = {
		{VK_KHR_SWAPCHAIN_EXTENSION_NAME, true},
	};
	TVector<const char*> ValidatedExtensions;
	ValidateExtensions(Extensions, &ValidatedExtensions);

	VkPhysicalDeviceDynamicRenderingFeatures DynamicRenderingFeatures = {};
	DynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
	DynamicRenderingFeatures.dynamicRendering = VK_TRUE;

	VkPhysicalDeviceSynchronization2Features Synchronization2Features = {};
	Synchronization2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	Synchronization2Features.pNext = &DynamicRenderingFeatures;
	Synchronization2Features.synchronization2 = VK_TRUE;

	VkPhysicalDeviceBufferDeviceAddressFeatures BufferDeviceAddressFeatures = {};
	BufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	BufferDeviceAddressFeatures.pNext = &Synchronization2Features;
	BufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;

	VkPhysicalDeviceFeatures2 Features = {};
	Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	Features.pNext = &BufferDeviceAddressFeatures;
	Features.features.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo DeviceCreateInfo = {};
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.pNext = &Features;
	DeviceCreateInfo.flags = 0;
	DeviceCreateInfo.pEnabledFeatures = nullptr;
	DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(QueueCreateInfos.size());
	DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
	DeviceCreateInfo.enabledLayerCount = static_cast<uint32>(ValidatedLayers.size());
	DeviceCreateInfo.ppEnabledLayerNames = ValidatedLayers.data();
	DeviceCreateInfo.enabledExtensionCount = static_cast<uint32>(ValidatedExtensions.size());
	DeviceCreateInfo.ppEnabledExtensionNames = ValidatedExtensions.data();

	VkResult CreateDeviceResult = vkCreateDevice(Description.Adapter->GetHandle(), &DeviceCreateInfo, Description.Context->GetAllocator(), &Device);
	CHECK_VKRESULT(CreateDeviceResult, "Failed to create Vulkan device");

	volkLoadDevice(Device);

	return RESULT_SUCCESS;
}

void FVulkanDevice::Destroy()
{
	if (Device)
	{
		FResult WaitResult = WaitForIdle();
		if (WaitResult.IsError()) [[unlikely]]
			RW_LOG_ERROR("Failed to wait for Vulkan device to become idle: {}", WaitResult.GetReason());
	}

	vkDestroyDevice(Device, Description.Context->GetAllocator());
	Description.Adapter.Reset();
	Description.Context.Reset();
}

FResult FVulkanDevice::WaitForIdle() const
{
	CHECK_VKRESULT(vkDeviceWaitIdle(Device), "Failed to wait for Vulkan device to become idle");
	return RESULT_SUCCESS;
}

TVector<VkLayerProperties> FVulkanDevice::AcquireAvailableLayers()
{
	uint32 LayerCount = 0;
	vkEnumerateDeviceLayerProperties(Description.Adapter->GetHandle(), &LayerCount, nullptr);
	TVector<VkLayerProperties> Layers(LayerCount);
	vkEnumerateDeviceLayerProperties(Description.Adapter->GetHandle(), &LayerCount, Layers.data());
	return Layers;
}

TVector<VkExtensionProperties> FVulkanDevice::AcquireAvailableExtensions()
{
	uint32 ExtensionCount = 0;
	vkEnumerateDeviceExtensionProperties(Description.Adapter->GetHandle(), nullptr, &ExtensionCount, nullptr);
	TVector<VkExtensionProperties> Extensions(ExtensionCount);
	vkEnumerateDeviceExtensionProperties(Description.Adapter->GetHandle(), nullptr, &ExtensionCount, Extensions.data());
	return Extensions;
}

void FVulkanDevice::ValidateLayers(const TVector<FVulkanRequireableComponent>& Layers, TVector<const char*>* OutValidatedLayers)
{
	TVector<FVulkanComponent> AvailableLayers = VulkanMapLayersToComponents(AcquireAvailableLayers());
	*OutValidatedLayers = VulkanValidateComponents(Layers, AvailableLayers);
	RW_LOG_INFO("Activated Vulkan layers:");
	for (const FVulkanComponent& Layer : Layers)
		RW_LOG_INFO("\t- {}", Layer.Name);
}

void FVulkanDevice::ValidateExtensions(const TVector<FVulkanRequireableComponent>& Extensions, TVector<const char*>* OutValidatedExtensions)
{
	TVector<FVulkanComponent> AvailableExtensions = VulkanMapExtensionsToComponents(AcquireAvailableExtensions());
	*OutValidatedExtensions = VulkanValidateComponents(Extensions, AvailableExtensions);
	RW_LOG_INFO("Activated Vulkan extensions:");
	for (const FVulkanComponent& Extension : Extensions)
		RW_LOG_INFO("\t- {}", Extension.Name);
}
