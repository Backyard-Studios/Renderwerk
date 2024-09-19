#include "pch.h"

#include "Renderwerk/Graphics/VulkanAdapter.h"

#include "Renderwerk/Graphics/Renderer.h"
#include "Renderwerk/Graphics/VulkanUtils.h"

FVulkanAdapter::FVulkanAdapter(const VkPhysicalDevice& InPhysicalDevice)
	: PhysicalDevice(InPhysicalDevice)
{
}

FVulkanAdapter::~FVulkanAdapter() = default;

FResult FVulkanAdapter::Initialize()
{
	vkGetPhysicalDeviceProperties(PhysicalDevice, &DeviceProperties);
	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &MemoryProperties);

	Features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	Features13.pNext = nullptr;

	Features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	Features12.pNext = &Features13;

	Features11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	Features11.pNext = &Features12;

	VkPhysicalDeviceFeatures2 TempFeatures = {};
	TempFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	TempFeatures.pNext = &Features11;
	vkGetPhysicalDeviceFeatures2(PhysicalDevice, &TempFeatures);
	Features = TempFeatures.features;

	return RW_RESULT_CODE_SUCCESS;
}

FResult FVulkanAdapter::QueryQueueData(const VkSurfaceKHR& Surface, const FVulkanQueueRequirements& Requirements, FVulkanQueueData* OutQueueData) const
{
	RW_ASSERT(OutQueueData != nullptr, RW_RESULT_CODE_INVALID_OBJECT, "OutQueueData is nullptr")

	uint32 QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);
	TVector<VkQueueFamilyProperties> QueueFamilyProperties(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilyProperties.data());

	uint8 BestTransferScore = 0;
	for (uint32 Index = 0; Index < QueueFamilyCount; ++Index)
	{
		uint8 CurrentTransferScore = 0;
		VkQueueFamilyProperties QueueFamily = QueueFamilyProperties[Index];

		if (OutQueueData->GraphicsQueueIndex == UINT32_MAX && QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			OutQueueData->GraphicsQueueIndex = Index;

			VkBool32 SupportsPresent = VK_FALSE;
			RW_CHECK_VKRESULT(vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, Index, Surface, &SupportsPresent), "Failed to query present support for queue family")
			if (SupportsPresent)
				OutQueueData->PresentQueueIndex = Index;

			if (Requirements.bRequireDedicatedGraphicsQueue)
				continue;
		}

		if (OutQueueData->ComputeQueueIndex == UINT32_MAX && QueueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			OutQueueData->ComputeQueueIndex = Index;
			++CurrentTransferScore;
		}

		if (Requirements.bUseBestQueueForTransport)
		{
			if (QueueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				if (CurrentTransferScore <= BestTransferScore)
				{
					BestTransferScore = CurrentTransferScore;
					OutQueueData->TransferQueueIndex = Index;
				}
			}
		}
		else
		{
			if (OutQueueData->TransferQueueIndex == UINT32_MAX && QueueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
				OutQueueData->TransferQueueIndex = Index;
		}
	}

	if (OutQueueData->PresentQueueIndex == UINT32_MAX)
	{
		for (uint32 Index = 0; Index < QueueFamilyCount; ++Index)
		{
			VkBool32 SupportsPresent = VK_FALSE;
			RW_CHECK_VKRESULT(vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, Index, Surface, &SupportsPresent), "Failed to query present support for queue family")
			if (SupportsPresent)
			{
				OutQueueData->PresentQueueIndex = Index;
				break;
			}
		}
	}

	if (!OutQueueData->IsGraphicsSharingWithPresent())
		RW_LOG_WARN("Graphics queue is not sharing with present queue! Querying for a new present queue...");

	if (!OutQueueData->IsValid())
		return FResult(RW_RESULT_CODE_INVALID_OBJECT, "Failed to find suitable queue family for Vulkan adapter");

	return RW_RESULT_CODE_SUCCESS;
}

FResult FVulkanAdapter::QuerySurfaceProperties(const VkSurfaceKHR& Surface, FVulkanSurfaceProperties* OutSurfaceProperties) const
{
	RW_ASSERT(OutSurfaceProperties != nullptr, RW_RESULT_CODE_INVALID_OBJECT, "OutSurfaceProperties is nullptr")

	RW_CHECK_VKRESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &OutSurfaceProperties->Capabilities), "Failed to query surface capabilities")

	uint32 FormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, nullptr);
	OutSurfaceProperties->Formats.resize(FormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, OutSurfaceProperties->Formats.data());

	uint32 PresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, nullptr);
	OutSurfaceProperties->PresentModes.resize(PresentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, OutSurfaceProperties->PresentModes.data());

	return RW_RESULT_CODE_SUCCESS;
}

bool8 FVulkanAdapter::SupportsLayers(const TVector<FVulkanRequireableComponent>& Layers) const
{
	uint32 LayerCount = 0;
	vkEnumerateDeviceLayerProperties(PhysicalDevice, &LayerCount, nullptr);
	TVector<VkLayerProperties> AvailableLayers(LayerCount);
	vkEnumerateDeviceLayerProperties(PhysicalDevice, &LayerCount, AvailableLayers.data());
	for (FVulkanRequireableComponent Layer : Layers)
	{
		bool bIsFound = false;
		for (VkLayerProperties AvailableLayer : AvailableLayers)
		{
			if (strcmp(Layer.Name, AvailableLayer.layerName) == 0)
			{
				bIsFound = true;
				break;
			}
		}
		if (!bIsFound && Layer.bIsRequired)
			return false;
	}
	return true;
}

bool8 FVulkanAdapter::SupportsExtensions(const TVector<FVulkanRequireableComponent>& Extensions) const
{
	uint32 ExtensionCount = 0;
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, nullptr);
	TVector<VkExtensionProperties> AvailableExtensions(ExtensionCount);
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, AvailableExtensions.data());
	for (FVulkanRequireableComponent Extension : Extensions)
	{
		bool bIsFound = false;
		for (VkExtensionProperties AvailableExtension : AvailableExtensions)
		{
			if (strcmp(Extension.Name, AvailableExtension.extensionName) == 0)
			{
				bIsFound = true;
				break;
			}
		}
		if (!bIsFound && Extension.bIsRequired)
			return false;
	}
	return true;
}

bool8 FVulkanAdapter::SupportsFeatures(const VkPhysicalDeviceFeatures& InFeatures) const
{
	if (InFeatures.robustBufferAccess && !Features.robustBufferAccess) return false;
	if (InFeatures.fullDrawIndexUint32 && !Features.fullDrawIndexUint32) return false;
	if (InFeatures.imageCubeArray && !Features.imageCubeArray) return false;
	if (InFeatures.independentBlend && !Features.independentBlend) return false;
	if (InFeatures.geometryShader && !Features.geometryShader) return false;
	if (InFeatures.tessellationShader && !Features.tessellationShader) return false;
	if (InFeatures.sampleRateShading && !Features.sampleRateShading) return false;
	if (InFeatures.dualSrcBlend && !Features.dualSrcBlend) return false;
	if (InFeatures.logicOp && !Features.logicOp) return false;
	if (InFeatures.multiDrawIndirect && !Features.multiDrawIndirect) return false;
	if (InFeatures.drawIndirectFirstInstance && !Features.drawIndirectFirstInstance) return false;
	if (InFeatures.depthClamp && !Features.depthClamp) return false;
	if (InFeatures.depthBiasClamp && !Features.depthBiasClamp) return false;
	if (InFeatures.fillModeNonSolid && !Features.fillModeNonSolid) return false;
	if (InFeatures.depthBounds && !Features.depthBounds) return false;
	if (InFeatures.wideLines && !Features.wideLines) return false;
	if (InFeatures.largePoints && !Features.largePoints) return false;
	if (InFeatures.alphaToOne && !Features.alphaToOne) return false;
	if (InFeatures.multiViewport && !Features.multiViewport) return false;
	if (InFeatures.samplerAnisotropy && !Features.samplerAnisotropy) return false;
	if (InFeatures.textureCompressionETC2 && !Features.textureCompressionETC2) return false;
	if (InFeatures.textureCompressionASTC_LDR && !Features.textureCompressionASTC_LDR) return false;
	if (InFeatures.textureCompressionBC && !Features.textureCompressionBC) return false;
	if (InFeatures.occlusionQueryPrecise && !Features.occlusionQueryPrecise) return false;
	if (InFeatures.pipelineStatisticsQuery && !Features.pipelineStatisticsQuery) return false;
	if (InFeatures.vertexPipelineStoresAndAtomics && !Features.vertexPipelineStoresAndAtomics) return false;
	if (InFeatures.fragmentStoresAndAtomics && !Features.fragmentStoresAndAtomics) return false;
	if (InFeatures.shaderTessellationAndGeometryPointSize && !Features.shaderTessellationAndGeometryPointSize) return false;
	if (InFeatures.shaderImageGatherExtended && !Features.shaderImageGatherExtended) return false;
	if (InFeatures.shaderStorageImageExtendedFormats && !Features.shaderStorageImageExtendedFormats) return false;
	if (InFeatures.shaderStorageImageMultisample && !Features.shaderStorageImageMultisample) return false;
	if (InFeatures.shaderStorageImageReadWithoutFormat && !Features.shaderStorageImageReadWithoutFormat) return false;
	if (InFeatures.shaderStorageImageWriteWithoutFormat && !Features.shaderStorageImageWriteWithoutFormat) return false;
	if (InFeatures.shaderUniformBufferArrayDynamicIndexing && !Features.shaderUniformBufferArrayDynamicIndexing) return false;
	if (InFeatures.shaderSampledImageArrayDynamicIndexing && !Features.shaderSampledImageArrayDynamicIndexing) return false;
	if (InFeatures.shaderStorageBufferArrayDynamicIndexing && !Features.shaderStorageBufferArrayDynamicIndexing) return false;
	if (InFeatures.shaderStorageImageArrayDynamicIndexing && !Features.shaderStorageImageArrayDynamicIndexing) return false;
	if (InFeatures.shaderClipDistance && !Features.shaderClipDistance) return false;
	if (InFeatures.shaderCullDistance && !Features.shaderCullDistance) return false;
	if (InFeatures.shaderFloat64 && !Features.shaderFloat64) return false;
	if (InFeatures.shaderInt64 && !Features.shaderInt64) return false;
	if (InFeatures.shaderInt16 && !Features.shaderInt16) return false;
	if (InFeatures.shaderResourceResidency && !Features.shaderResourceResidency) return false;
	if (InFeatures.shaderResourceMinLod && !Features.shaderResourceMinLod) return false;
	if (InFeatures.sparseBinding && !Features.sparseBinding) return false;
	if (InFeatures.sparseResidencyBuffer && !Features.sparseResidencyBuffer) return false;
	if (InFeatures.sparseResidencyImage2D && !Features.sparseResidencyImage2D) return false;
	if (InFeatures.sparseResidencyImage3D && !Features.sparseResidencyImage3D) return false;
	if (InFeatures.sparseResidency2Samples && !Features.sparseResidency2Samples) return false;
	if (InFeatures.sparseResidency4Samples && !Features.sparseResidency4Samples) return false;
	if (InFeatures.sparseResidency8Samples && !Features.sparseResidency8Samples) return false;
	if (InFeatures.sparseResidency16Samples && !Features.sparseResidency16Samples) return false;
	if (InFeatures.sparseResidencyAliased && !Features.sparseResidencyAliased) return false;
	if (InFeatures.variableMultisampleRate && !Features.variableMultisampleRate) return false;
	if (InFeatures.inheritedQueries && !Features.inheritedQueries) return false;
	return true;
}

bool8 FVulkanAdapter::SupportsFeatures11(const VkPhysicalDeviceVulkan11Features& InFeatures) const
{
	if (InFeatures.storageBuffer16BitAccess && !Features11.storageBuffer16BitAccess) return false;
	if (InFeatures.uniformAndStorageBuffer16BitAccess && !Features11.uniformAndStorageBuffer16BitAccess) return false;
	if (InFeatures.storagePushConstant16 && !Features11.storagePushConstant16) return false;
	if (InFeatures.storageInputOutput16 && !Features11.storageInputOutput16) return false;
	if (InFeatures.multiview && !Features11.multiview) return false;
	if (InFeatures.multiviewGeometryShader && !Features11.multiviewGeometryShader) return false;
	if (InFeatures.multiviewTessellationShader && !Features11.multiviewTessellationShader) return false;
	if (InFeatures.variablePointersStorageBuffer && !Features11.variablePointersStorageBuffer) return false;
	if (InFeatures.variablePointers && !Features11.variablePointers) return false;
	if (InFeatures.protectedMemory && !Features11.protectedMemory) return false;
	if (InFeatures.samplerYcbcrConversion && !Features11.samplerYcbcrConversion) return false;
	if (InFeatures.shaderDrawParameters && !Features11.shaderDrawParameters) return false;
	return true;
}

bool8 FVulkanAdapter::SupportsFeatures12(const VkPhysicalDeviceVulkan12Features& InFeatures) const
{
	if (InFeatures.samplerMirrorClampToEdge && !Features12.samplerMirrorClampToEdge) return false;
	if (InFeatures.drawIndirectCount && !Features12.drawIndirectCount) return false;
	if (InFeatures.storageBuffer8BitAccess && !Features12.storageBuffer8BitAccess) return false;
	if (InFeatures.uniformAndStorageBuffer8BitAccess && !Features12.uniformAndStorageBuffer8BitAccess) return false;
	if (InFeatures.storagePushConstant8 && !Features12.storagePushConstant8) return false;
	if (InFeatures.shaderBufferInt64Atomics && !Features12.shaderBufferInt64Atomics) return false;
	if (InFeatures.shaderSharedInt64Atomics && !Features12.shaderSharedInt64Atomics) return false;
	if (InFeatures.shaderFloat16 && !Features12.shaderFloat16) return false;
	if (InFeatures.shaderInt8 && !Features12.shaderInt8) return false;
	if (InFeatures.descriptorIndexing && !Features12.descriptorIndexing) return false;
	if (InFeatures.shaderInputAttachmentArrayDynamicIndexing && !Features12.shaderInputAttachmentArrayDynamicIndexing) return false;
	if (InFeatures.shaderUniformTexelBufferArrayDynamicIndexing && !Features12.shaderUniformTexelBufferArrayDynamicIndexing) return false;
	if (InFeatures.shaderStorageTexelBufferArrayDynamicIndexing && !Features12.shaderStorageTexelBufferArrayDynamicIndexing) return false;
	if (InFeatures.shaderUniformBufferArrayNonUniformIndexing && !Features12.shaderUniformBufferArrayNonUniformIndexing) return false;
	if (InFeatures.shaderSampledImageArrayNonUniformIndexing && !Features12.shaderSampledImageArrayNonUniformIndexing) return false;
	if (InFeatures.shaderStorageBufferArrayNonUniformIndexing && !Features12.shaderStorageBufferArrayNonUniformIndexing) return false;
	if (InFeatures.shaderStorageImageArrayNonUniformIndexing && !Features12.shaderStorageImageArrayNonUniformIndexing) return false;
	if (InFeatures.shaderInputAttachmentArrayNonUniformIndexing && !Features12.shaderInputAttachmentArrayNonUniformIndexing) return false;
	if (InFeatures.shaderUniformTexelBufferArrayNonUniformIndexing && !Features12.shaderUniformTexelBufferArrayNonUniformIndexing) return false;
	if (InFeatures.shaderStorageTexelBufferArrayNonUniformIndexing && !Features12.shaderStorageTexelBufferArrayNonUniformIndexing) return false;
	if (InFeatures.descriptorBindingUniformBufferUpdateAfterBind && !Features12.descriptorBindingUniformBufferUpdateAfterBind) return false;
	if (InFeatures.descriptorBindingSampledImageUpdateAfterBind && !Features12.descriptorBindingSampledImageUpdateAfterBind) return false;
	if (InFeatures.descriptorBindingStorageImageUpdateAfterBind && !Features12.descriptorBindingStorageImageUpdateAfterBind) return false;
	if (InFeatures.descriptorBindingStorageBufferUpdateAfterBind && !Features12.descriptorBindingStorageBufferUpdateAfterBind) return false;
	if (InFeatures.descriptorBindingUniformTexelBufferUpdateAfterBind && !Features12.descriptorBindingUniformTexelBufferUpdateAfterBind) return false;
	if (InFeatures.descriptorBindingStorageTexelBufferUpdateAfterBind && !Features12.descriptorBindingStorageTexelBufferUpdateAfterBind) return false;
	if (InFeatures.descriptorBindingUpdateUnusedWhilePending && !Features12.descriptorBindingUpdateUnusedWhilePending) return false;
	if (InFeatures.descriptorBindingPartiallyBound && !Features12.descriptorBindingPartiallyBound) return false;
	if (InFeatures.descriptorBindingVariableDescriptorCount && !Features12.descriptorBindingVariableDescriptorCount) return false;
	if (InFeatures.runtimeDescriptorArray && !Features12.runtimeDescriptorArray) return false;
	if (InFeatures.samplerFilterMinmax && !Features12.samplerFilterMinmax) return false;
	if (InFeatures.scalarBlockLayout && !Features12.scalarBlockLayout) return false;
	if (InFeatures.imagelessFramebuffer && !Features12.imagelessFramebuffer) return false;
	if (InFeatures.uniformBufferStandardLayout && !Features12.uniformBufferStandardLayout) return false;
	if (InFeatures.shaderSubgroupExtendedTypes && !Features12.shaderSubgroupExtendedTypes) return false;
	if (InFeatures.separateDepthStencilLayouts && !Features12.separateDepthStencilLayouts) return false;
	if (InFeatures.hostQueryReset && !Features12.hostQueryReset) return false;
	if (InFeatures.timelineSemaphore && !Features12.timelineSemaphore) return false;
	if (InFeatures.bufferDeviceAddress && !Features12.bufferDeviceAddress) return false;
	if (InFeatures.bufferDeviceAddressCaptureReplay && !Features12.bufferDeviceAddressCaptureReplay) return false;
	if (InFeatures.bufferDeviceAddressMultiDevice && !Features12.bufferDeviceAddressMultiDevice) return false;
	if (InFeatures.vulkanMemoryModel && !Features12.vulkanMemoryModel) return false;
	if (InFeatures.vulkanMemoryModelDeviceScope && !Features12.vulkanMemoryModelDeviceScope) return false;
	if (InFeatures.vulkanMemoryModelAvailabilityVisibilityChains && !Features12.vulkanMemoryModelAvailabilityVisibilityChains) return false;
	if (InFeatures.shaderOutputViewportIndex && !Features12.shaderOutputViewportIndex) return false;
	if (InFeatures.shaderOutputLayer && !Features12.shaderOutputLayer) return false;
	if (InFeatures.subgroupBroadcastDynamicId && !Features12.subgroupBroadcastDynamicId) return false;
	return true;
}

bool8 FVulkanAdapter::SupportsFeatures13(const VkPhysicalDeviceVulkan13Features& InFeatures) const
{
	if (InFeatures.robustImageAccess && !Features13.robustImageAccess) return false;
	if (InFeatures.inlineUniformBlock && !Features13.inlineUniformBlock) return false;
	if (InFeatures.descriptorBindingInlineUniformBlockUpdateAfterBind && !Features13.descriptorBindingInlineUniformBlockUpdateAfterBind) return false;
	if (InFeatures.pipelineCreationCacheControl && !Features13.pipelineCreationCacheControl) return false;
	if (InFeatures.privateData && !Features13.privateData) return false;
	if (InFeatures.shaderDemoteToHelperInvocation && !Features13.shaderDemoteToHelperInvocation) return false;
	if (InFeatures.shaderTerminateInvocation && !Features13.shaderTerminateInvocation) return false;
	if (InFeatures.subgroupSizeControl && !Features13.subgroupSizeControl) return false;
	if (InFeatures.computeFullSubgroups && !Features13.computeFullSubgroups) return false;
	if (InFeatures.synchronization2 && !Features13.synchronization2) return false;
	if (InFeatures.textureCompressionASTC_HDR && !Features13.textureCompressionASTC_HDR) return false;
	if (InFeatures.shaderZeroInitializeWorkgroupMemory && !Features13.shaderZeroInitializeWorkgroupMemory) return false;
	if (InFeatures.dynamicRendering && !Features13.dynamicRendering) return false;
	if (InFeatures.shaderIntegerDotProduct && !Features13.shaderIntegerDotProduct) return false;
	if (InFeatures.maintenance4 && !Features13.maintenance4) return false;
	return true;
}
