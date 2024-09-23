#include "pch.h"

#include "Renderwerk/Graphics/VulkanAdapter.h"

#include "Renderwerk/Graphics/VulkanUtils.h"

std::string ToString(const EVulkanVendor& Vendor)
{
	switch (Vendor)
	{
	case EVulkanVendor::AMD: return "AMD";
	case EVulkanVendor::NVIDIA: return "NVIDIA";
	case EVulkanVendor::Intel: return "Intel";
	case EVulkanVendor::Unknown:
	default:
		return "Unknown";
	}
}

FVulkanAdapter::FVulkanAdapter(const VkPhysicalDevice& InPhysicalDevice, const uint32 InIndex)
	: PhysicalDevice(InPhysicalDevice), Index(InIndex)
{
}

FVulkanAdapter::~FVulkanAdapter() = default;

FResult FVulkanAdapter::Initialize()
{
	vkGetPhysicalDeviceProperties(PhysicalDevice, &Properties);
	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &MemoryProperties);

	Core13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	Core12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	Core12Features.pNext = &Core13Features;
	Core11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	Core11Features.pNext = &Core12Features;

	VkPhysicalDeviceFeatures2 TempFeatures = {};
	TempFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	TempFeatures.pNext = &Core11Features;
	vkGetPhysicalDeviceFeatures2(PhysicalDevice, &TempFeatures);
	Features = TempFeatures.features;

	return RESULT_SUCCESS;
}

bool8 FVulkanAdapter::IsDiscrete() const
{
	return Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

EVulkanVendor FVulkanAdapter::GetVendor() const
{
#define DEFINE_CASE(Vendor) case static_cast<uint32>(EVulkanVendor::Vendor): return EVulkanVendor::Vendor
	switch (Properties.vendorID)
	{
	DEFINE_CASE(AMD);
	DEFINE_CASE(NVIDIA);
	DEFINE_CASE(Intel);
	default:
		return EVulkanVendor::Unknown;
	}
#undef DEFINE_CASE
}

FResult FVulkanAdapter::QuerySurfaceCapabilities(const VkSurfaceKHR& Surface, VkSurfaceCapabilitiesKHR* OutCapabilities) const
{
	CHECK_VKRESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, OutCapabilities), "Failed to query surface capabilities");
	return RESULT_SUCCESS;
}

FResult FVulkanAdapter::QuerySurfaceProperties(const VkSurfaceKHR& Surface, FVulkanSurfaceProperties* OutProperties) const
{
	CHECK_RESULT(QuerySurfaceCapabilities(Surface, &OutProperties->Capabilities))

	uint32 FormatCount = 0;
	CHECK_VKRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, nullptr), "Failed to query surface formats");
	OutProperties->Formats.resize(FormatCount);
	CHECK_VKRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, OutProperties->Formats.data()), "Failed to query surface formats");

	uint32 PresentModeCount = 0;
	CHECK_VKRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, nullptr), "Failed to query present modes");
	OutProperties->PresentModes.resize(PresentModeCount);
	CHECK_VKRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, OutProperties->PresentModes.data()),
	               "Failed to query present modes");

	return RESULT_SUCCESS;
}

FResult FVulkanAdapter::QueryQueueData(const VkSurfaceKHR& Surface, FVulkanQueueData* OutQueueData) const
{
	uint32 QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);
	TVector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilies.data());

	uint8 BestTransferScore = 0;
	for (uint32 Index = 0; Index < QueueFamilyCount; ++Index)
	{
		VkQueueFamilyProperties QueueFamily = QueueFamilies[Index];
		uint8 CurrentTransferScore = 0;

		if (OutQueueData->GraphicsIndex == UINT32_MAX && QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			OutQueueData->GraphicsIndex = Index;
			++CurrentTransferScore;

			if (OutQueueData->PresentIndex == UINT32_MAX)
			{
				VkBool32 SupportsPresent = VK_FALSE;
				CHECK_VKRESULT(vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, Index, Surface, &SupportsPresent), "Failed to query surface support");
				if (SupportsPresent)
				{
					OutQueueData->PresentIndex = Index;
					++CurrentTransferScore;
				}
			}
		}

		if (QueueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			OutQueueData->ComputeIndex = Index;
			++CurrentTransferScore;
		}

		if (QueueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			if (CurrentTransferScore <= BestTransferScore)
			{
				BestTransferScore = CurrentTransferScore;
				OutQueueData->TransferIndex = Index;
			}
		}
	}
	if (!OutQueueData->IsValid())
		return FResult(RESULT_FAILED, "Failed to find suitable queue families");
	return RESULT_SUCCESS;
}
