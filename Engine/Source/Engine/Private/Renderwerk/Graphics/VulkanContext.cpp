#include "pch.h"

#include "Renderwerk/Graphics/VulkanContext.h"

#include "Renderwerk/Graphics/Renderer.h"
#include "Renderwerk/Graphics/VulkanUtils.h"

VkBool32 DebugUtilsMessengerCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT Severity, VkDebugUtilsMessageTypeFlagsEXT Type,
                                     const VkDebugUtilsMessengerCallbackDataEXT* CallbackData, void* UserData)
{
	switch (Severity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		RW_LOG_TRACE("Vulkan Verbose: {0}", CallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		RW_LOG_INFO("Vulkan Info: {0}", CallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		RW_LOG_WARN("Vulkan Warning: {0}", CallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		{
			std::string Message = CallbackData->pMessage;
			size64 WallPos = Message.find_first_of("|");
			if (WallPos != std::string::npos)
			{
				Message = Message.substr(WallPos + 1);
				WallPos = Message.find_first_of("|");
				if (WallPos != std::string::npos)
				{
					Message = Message.substr(WallPos + 2);
					size64 SpecPos = Message.find_first_of(".");
					if (SpecPos != std::string::npos)
						Message = Message.substr(0, SpecPos);
				}
			}
			RW_LOG_ERROR("Vulkan Error: {0}", Message);
		}
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
	default:
		break;
	}
	return VK_FALSE;
}

FVulkanContext::FVulkanContext(const FVulkanContextSettings& InSettings)
	: Settings(InSettings)
{
}

FVulkanContext::~FVulkanContext() = default;


FResult FVulkanContext::Initialize()
{
	if (Settings.bEnableValidation)
		RW_LOG_WARN("Validation layers are enabled. This may impact performance");
	if (Settings.bEnableDebugging)
		RW_LOG_DEBUG("Debugging is enabled");

	RW_CHECK_RESULT(CreateInstance());

	RW_LOG_DEBUG("Activated Instance Layers:");
	for (FVulkanComponent ActivatedLayer : ActivatedLayers)
		RW_LOG_DEBUG("\t- {} (Spec Version: {}, Implementation Version: {})", ActivatedLayer.Name, VulkanFormatVersion(ActivatedLayer.SpecVersion),
	             ActivatedLayer.ImplementationVersion);

	RW_LOG_DEBUG("Activated Instance Extensions:");
	for (FVulkanComponent ActivatedExtension : ActivatedExtensions)
		RW_LOG_DEBUG("\t- {} (Spec Version: {})", ActivatedExtension.Name, VulkanFormatVersion(ActivatedExtension.SpecVersion));

	if (Settings.bEnableDebugging)
		RW_CHECK_RESULT(CreateDebugMessenger());
	return RW_RESULT_CODE_SUCCESS;
}

void FVulkanContext::Destroy() const
{
	if (Settings.bEnableDebugging)
	{
		PFN_vkDestroyDebugUtilsMessengerEXT DestroyMessengerFunc = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
			Instance, "vkDestroyDebugUtilsMessengerEXT"));
		DestroyMessengerFunc(Instance, DebugMessenger, Settings.Allocator);
	}
	vkDestroyInstance(Instance, Settings.Allocator);
}

FResult FVulkanContext::CreateInstance()
{
	VkApplicationInfo ApplicationInfo;
	ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	ApplicationInfo.pNext = nullptr;
	ApplicationInfo.pApplicationName = "Renderwerk";
	ApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	ApplicationInfo.pEngineName = "Renderwerk";
	ApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	ApplicationInfo.apiVersion = Settings.APIVersion;

	if (Settings.bEnableValidation)
		Settings.RequestedLayers.push_back({"VK_LAYER_KHRONOS_validation", true});

	if (Settings.bEnableDebugging)
		Settings.RequestedExtensions.push_back({"VK_EXT_debug_utils", true});

	TVector<const char*> ValidatedLayers;
	RW_CHECK_RESULT(ValidateRequestedLayers(&ValidatedLayers));

	TVector<const char*> ValidatedExtensions;
	RW_CHECK_RESULT(ValidateRequestedExtensions(&ValidatedExtensions));

	VkInstanceCreateInfo InstanceCreateInfo;
	InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceCreateInfo.pNext = nullptr;
	InstanceCreateInfo.flags = 0;
	InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
	InstanceCreateInfo.enabledLayerCount = static_cast<uint32>(ValidatedLayers.size());
	InstanceCreateInfo.ppEnabledLayerNames = ValidatedLayers.data();
	InstanceCreateInfo.enabledExtensionCount = static_cast<uint32>(ValidatedExtensions.size());
	InstanceCreateInfo.ppEnabledExtensionNames = ValidatedExtensions.data();

	RW_CHECK_VKRESULT(vkCreateInstance(&InstanceCreateInfo, Settings.Allocator, &Instance), "Failed to create Vulkan instance.");

	return RW_RESULT_CODE_SUCCESS;
}

FResult FVulkanContext::CreateDebugMessenger()
{
	VkDebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo;
	DebugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	DebugMessengerCreateInfo.pNext = nullptr;
	DebugMessengerCreateInfo.flags = 0;
	DebugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	DebugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	DebugMessengerCreateInfo.pfnUserCallback = DebugUtilsMessengerCallback;
	DebugMessengerCreateInfo.pUserData = nullptr;

	PFN_vkCreateDebugUtilsMessengerEXT CreateMessengerFunc = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
		Instance, "vkCreateDebugUtilsMessengerEXT"));
	CreateMessengerFunc(Instance, &DebugMessengerCreateInfo, Settings.Allocator, &DebugMessenger);
	return RW_RESULT_CODE_SUCCESS;
}

FResult FVulkanContext::ValidateRequestedLayers(TVector<const char*>* OutValidatedLayers)
{
	uint32 LayerCount = 0;
	RW_CHECK_VKRESULT(vkEnumerateInstanceLayerProperties(&LayerCount, nullptr), "Failed to enumerate Vulkan layers.");
	TVector<VkLayerProperties> AvailableLayers(LayerCount);
	RW_CHECK_VKRESULT(vkEnumerateInstanceLayerProperties(&LayerCount, AvailableLayers.data()), "Failed to enumerate Vulkan layers.");
	for (FVulkanRequireableComponent RequestedLayer : Settings.RequestedLayers)
	{
		bool8 bIsAvailable = false;
		for (VkLayerProperties AvailableLayer : AvailableLayers)
		{
			if (strcmp(RequestedLayer.Name, AvailableLayer.layerName) == 0)
			{
				bIsAvailable = true;
				OutValidatedLayers->push_back(RequestedLayer.Name);
				RequestedLayer.SpecVersion = AvailableLayer.specVersion;
				RequestedLayer.ImplementationVersion = AvailableLayer.implementationVersion;
				ActivatedLayers.push_back(RequestedLayer);
				break;
			}
		}
		if (!bIsAvailable && RequestedLayer.bIsRequired)
			return FResult(RW_RESULT_CODE_FAIL, std::format("Required layer \"{}\" not available.", RequestedLayer.Name));
		if (!bIsAvailable)
			RW_LOG_WARN("Layer \"{}\" not available.", RequestedLayer.Name);
	}
	return RW_RESULT_CODE_SUCCESS;
}

FResult FVulkanContext::ValidateRequestedExtensions(TVector<const char*>* OutValidatedExtensions)
{
	uint32 ExtensionCount = 0;
	RW_CHECK_VKRESULT(vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr), "Failed to enumerate Vulkan extensions.");
	TVector<VkExtensionProperties> AvailableExtensions(ExtensionCount);
	RW_CHECK_VKRESULT(vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, AvailableExtensions.data()), "Failed to enumerate Vulkan extensions.");
	for (FVulkanRequireableComponent RequestedExtension : Settings.RequestedExtensions)
	{
		bool8 bIsAvailable = false;
		for (VkExtensionProperties AvailableExtension : AvailableExtensions)
		{
			if (strcmp(RequestedExtension.Name, AvailableExtension.extensionName) == 0)
			{
				bIsAvailable = true;
				OutValidatedExtensions->push_back(RequestedExtension.Name);
				RequestedExtension.SpecVersion = AvailableExtension.specVersion;
				RequestedExtension.ImplementationVersion = AvailableExtension.specVersion;
				ActivatedExtensions.push_back(RequestedExtension);
				break;
			}
		}
		if (!bIsAvailable && RequestedExtension.bIsRequired)
			return FResult(RW_RESULT_CODE_FAIL, std::format("Required extension \"{}\" not available.", RequestedExtension.Name));
		if (!bIsAvailable)
			RW_LOG_WARN("Extension \"{}\" not available.", RequestedExtension.Name);
	}
	return RW_RESULT_CODE_SUCCESS;
}
