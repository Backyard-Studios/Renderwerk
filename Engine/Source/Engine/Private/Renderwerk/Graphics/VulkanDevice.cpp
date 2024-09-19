#include "pch.h"

#include "Renderwerk/Graphics/VulkanDevice.h"

#include "Renderwerk/Graphics/Renderer.h"
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
	FVulkanQueueData QueueData = {};
	RW_CHECK_RESULT(Description.Adapter->QueryQueueData(Description.Surface, Description.QueueRequirements, &QueueData))

	TVector<uint32> QueueIndices;
	QueueIndices.push_back(QueueData.GraphicsQueueIndex);
	if (!QueueData.IsGraphicsSharingWithPresent())
		QueueIndices.push_back(QueueData.PresentQueueIndex);
	QueueIndices.push_back(QueueData.ComputeQueueIndex);
	QueueIndices.push_back(QueueData.TransferQueueIndex);

	TVector<VkDeviceQueueCreateInfo> QueueCreateInfos;

	constexpr float32 QueuePriorities[] = {0.9f, 1.0f};
	for (uint32 QueueIndex : QueueIndices)
	{
		VkDeviceQueueCreateInfo QueueCreateInfo = {};
		QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		QueueCreateInfo.pNext = nullptr;
		QueueCreateInfo.flags = 0;
		QueueCreateInfo.queueFamilyIndex = QueueIndex;
		if (QueueIndex == QueueData.GraphicsQueueIndex && QueueData.IsGraphicsSharingWithPresent())
			QueueCreateInfo.queueCount = 2;
		else
			QueueCreateInfo.queueCount = 1;
		QueueCreateInfo.pQueuePriorities = QueuePriorities;
		QueueCreateInfos.push_back(QueueCreateInfo);
	}

	ActivatedLayers = VulkanMapComponents(Description.RequiredLayers);
	ActivatedExtensions = VulkanMapComponents(Description.RequiredExtensions);

	VkDeviceCreateInfo DeviceCreateInfo;
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.pNext = Description.NextChain;
	DeviceCreateInfo.flags = 0;
	DeviceCreateInfo.pEnabledFeatures = &Description.EnabledFeatures;
	DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(QueueCreateInfos.size());
	DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
	DeviceCreateInfo.enabledLayerCount = static_cast<uint32>(ActivatedLayers.size());
	DeviceCreateInfo.ppEnabledLayerNames = ActivatedLayers.data();
	DeviceCreateInfo.enabledExtensionCount = static_cast<uint32>(ActivatedExtensions.size());
	DeviceCreateInfo.ppEnabledExtensionNames = ActivatedExtensions.data();

	RW_CHECK_VKRESULT(vkCreateDevice(Description.Adapter->GetHandle(), &DeviceCreateInfo, Description.Allocator, &Device), "Failed to create logical device")

	vkGetDeviceQueue(Device, QueueData.GraphicsQueueIndex, 0, &GraphicsQueue);
	if (!QueueData.IsGraphicsSharingWithPresent())
		vkGetDeviceQueue(Device, QueueData.PresentQueueIndex, 0, &PresentQueue);
	else
		vkGetDeviceQueue(Device, QueueData.GraphicsQueueIndex, 1, &PresentQueue);
	vkGetDeviceQueue(Device, QueueData.ComputeQueueIndex, 0, &ComputeQueue);
	vkGetDeviceQueue(Device, QueueData.TransferQueueIndex, 0, &TransferQueue);

	return RW_RESULT_CODE_SUCCESS;
}

void FVulkanDevice::Destroy() const
{
	if (Device)
	{
		FResult WaitResult = WaitForIdle();
		if (WaitResult != RW_RESULT_CODE_SUCCESS)
			RW_LOG_ERROR("Failed to wait for device to become idle");
	}

	vkDestroyDevice(Device, Description.Allocator);
}

FResult FVulkanDevice::WaitForIdle() const
{
	RW_CHECK_VKRESULT(vkDeviceWaitIdle(Device), "Failed to wait for device to become idle")
	return RW_RESULT_CODE_SUCCESS;
}
