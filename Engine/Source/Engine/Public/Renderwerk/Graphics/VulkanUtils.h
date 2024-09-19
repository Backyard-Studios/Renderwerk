#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include "vulkan/vulkan.h"

#define RW_CHECK_VKRESULT(ResultExpression, ...) \
	{ \
		VkResult UNIQUE_NAME(VulkanResult) = ResultExpression; \
		if (UNIQUE_NAME(VulkanResult) != VK_SUCCESS) \
			return FResult(RW_RESULT_CODE_VULKAN_RESULT_FAILED, std::format(__VA_ARGS__).c_str()); \
	}

ENGINE_API INLINE std::string VulkanAPIVersionToString(const uint32 APIVersion)
{
	switch (APIVersion)
	{
	case VK_API_VERSION_1_0: return "1.0";
	case VK_API_VERSION_1_1: return "1.1";
	case VK_API_VERSION_1_2: return "1.2";
	case VK_API_VERSION_1_3: return "1.3";
	default: return "Unknown";
	}
}

ENGINE_API INLINE std::string FormatVulkanVersion(const uint32 Version)
{
	return std::format("{}.{}.{}", VK_VERSION_MAJOR(Version), VK_VERSION_MINOR(Version), VK_VERSION_PATCH(Version));
}
