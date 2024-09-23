#include "pch.h"

#include "Renderwerk/Graphics/VulkanContext.h"

#include "Renderwerk/Graphics/VulkanTypes.h"

#define VOLK_IMPLEMENTATION
// ReSharper disable once CppUnusedIncludeDirective
#include "volk.h"

#include "Renderwerk/Graphics/VulkanUtils.h"

FVulkanContext::FVulkanContext(const FVulkanContextDesc& InDescription)
	: Description(InDescription)
{
}

FVulkanContext::~FVulkanContext() = default;

FResult FVulkanContext::Initialize()
{
	VkResult VolkInitializeResult = volkInitialize();
	CHECK_VKRESULT(VolkInitializeResult, "Failed to initialize Volk")

	uint32 VulkanVersion = volkGetInstanceVersion();
	if (VulkanVersion == 0)
		return FResult(RESULT_FAILED, "Vulkan is not supported");

	RW_LOG_INFO("Available Vulkan version: {}.{}.{}", VK_VERSION_MAJOR(VulkanVersion), VK_VERSION_MINOR(VulkanVersion), VK_VERSION_PATCH(VulkanVersion));

	Allocator = CreateAllocator();

	VkApplicationInfo ApplicationInfo = {};
	ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	ApplicationInfo.pApplicationName = "Renderwerk";
	ApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	ApplicationInfo.pEngineName = "Renderwerk";
	ApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	ApplicationInfo.apiVersion = VK_API_VERSION_1_3;

	TVector<FVulkanRequireableComponent> Layers = {};
	if (Description.bEnableValidationLayers)
		Layers.push_back({"VK_LAYER_KHRONOS_validation", false});
	TVector<const char*> ValidatedLayers;
	ValidateLayers(Layers, &ValidatedLayers);

	TVector<FVulkanRequireableComponent> Extensions = {
		{VK_KHR_SURFACE_EXTENSION_NAME, true},
		{"VK_KHR_win32_surface", true},
	};
#if RW_CONFIG_DEBUG
	Extensions.push_back({VK_EXT_DEBUG_UTILS_EXTENSION_NAME, true});
#endif
	TVector<FVulkanComponent> AvailableExtensions = VulkanMapExtensionsToComponents(AcquireAvailableExtensions());
	TVector<const char*> ValidatedExtensions;
	ValidateExtensions(Extensions, &ValidatedExtensions);

	VkInstanceCreateInfo InstanceCreateInfo = {};
	InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
	InstanceCreateInfo.enabledLayerCount = static_cast<uint32>(ValidatedLayers.size());
	InstanceCreateInfo.ppEnabledLayerNames = ValidatedLayers.data();
	InstanceCreateInfo.enabledExtensionCount = static_cast<uint32>(ValidatedExtensions.size());
	InstanceCreateInfo.ppEnabledExtensionNames = ValidatedExtensions.data();

	VkResult InstanceCreateResult = vkCreateInstance(&InstanceCreateInfo, Allocator, &Instance);
	CHECK_VKRESULT(InstanceCreateResult, "Failed to create Vulkan instance")
	RW_LOG_DEBUG("Created Vulkan instance");

	volkLoadInstanceOnly(Instance);

	return RESULT_SUCCESS;
}

void FVulkanContext::Destroy()
{
	vkDestroyInstance(Instance, Allocator);
	FMemory::Delete(Allocator);
	Allocator = nullptr;
	volkFinalize();
}

VkAllocationCallbacks* FVulkanContext::CreateAllocator()
{
	VkAllocationCallbacks* Allocator = FMemory::New<VkAllocationCallbacks>();
	Allocator->pUserData = Allocator;
	Allocator->pfnAllocation = []([[maybe_unused]] void* UserData, const size_t Size, const size_t Alignment, VkSystemAllocationScope AllocationScope) -> void* {
		if (Size == 0)
			return nullptr;
		return FMemory::Allocate(Size, static_cast<uint8>(Alignment));
	};
	Allocator->pfnFree = []([[maybe_unused]] void* UserData, void* Memory) -> void
	{
		if (!Memory)
			return;
		FMemory::Free(Memory);
	};
	// ReSharper disable once CppParameterMayBeConstPtrOrRef
	Allocator->pfnReallocation = [](void* UserData, void* Original, const size_t Size, const size_t Alignment, const VkSystemAllocationScope AllocationScope) -> void* {
		VkAllocationCallbacks* Allocator = static_cast<VkAllocationCallbacks*>(UserData);
		if (Original == nullptr)
			return Allocator->pfnAllocation(Allocator->pUserData, Size, Alignment, AllocationScope);
		if (Size == 0)
		{
			Allocator->pfnFree(Allocator->pUserData, Original);
			return nullptr;
		}

		void* NewMemory = FMemory::Reallocate(Original, Size, static_cast<uint8>(Alignment));
		return NewMemory;
	};
	Allocator->pfnInternalAllocation = nullptr;
	Allocator->pfnInternalFree = nullptr;
	return Allocator;
}

TVector<VkLayerProperties> FVulkanContext::AcquireAvailableLayers()
{
	uint32 LayerCount = 0;
	vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);
	TVector<VkLayerProperties> AvailableLayers(LayerCount);
	vkEnumerateInstanceLayerProperties(&LayerCount, AvailableLayers.data());
	return AvailableLayers;
}

TVector<VkExtensionProperties> FVulkanContext::AcquireAvailableExtensions()
{
	uint32 ExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);
	TVector<VkExtensionProperties> AvailableExtensions(ExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, AvailableExtensions.data());
	return AvailableExtensions;
}

void FVulkanContext::ValidateLayers(const TVector<FVulkanRequireableComponent>& Layers, TVector<const char*>* OutValidatedLayers)
{
	TVector<FVulkanComponent> AvailableLayers = VulkanMapLayersToComponents(AcquireAvailableLayers());
	*OutValidatedLayers = VulkanValidateComponents(Layers, AvailableLayers);
	RW_LOG_INFO("Activated Vulkan layers:");
	for (const FVulkanComponent& Layer : Layers)
		RW_LOG_INFO("\t- {}", Layer.Name);
}

void FVulkanContext::ValidateExtensions(const TVector<FVulkanRequireableComponent>& Extensions, TVector<const char*>* OutValidatedExtensions)
{
	TVector<FVulkanComponent> AvailableExtensions = VulkanMapExtensionsToComponents(AcquireAvailableExtensions());
	*OutValidatedExtensions = VulkanValidateComponents(Extensions, AvailableExtensions);
	RW_LOG_INFO("Activated Vulkan extensions:");
	for (const FVulkanComponent& Extension : Extensions)
		RW_LOG_INFO("\t- {}", Extension.Name);
}
