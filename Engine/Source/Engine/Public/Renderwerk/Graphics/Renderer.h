#pragma once

#include "VulkanAdapter.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Platform/Window.h"

#define RW_RESULT_CODE_VULKAN_RESULT_FAILED RW_MAKE_RESULT_CODE(RW_RESULT_SEVERITY_ERROR, 101)

struct ENGINE_API FRendererSettings
{
	TSharedPointer<IWindow> Window;
	bool bEnableValidation = false;
	bool bEnableDebugging = false;
};

struct ENGINE_API FRenderFrame
{
	VkCommandPool CommandPool = VK_NULL_HANDLE;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
	VkSemaphore ImageAvailableSemaphore = VK_NULL_HANDLE;
	VkSemaphore RenderFinishedSemaphore = VK_NULL_HANDLE;
	VkFence InFlightFence = VK_NULL_HANDLE;
};

class ENGINE_API FRenderer
{
public:
	FRenderer(const FRendererSettings& InSettings);
	~FRenderer();

	DELETE_COPY_AND_MOVE(FRenderer)

private:
	[[nodiscard]] FResult Initialize();
	void Shutdown();

public:
	FResult BeginFrame();
	FResult EndFrame();

private:
	FResult SelectSuitableAdapter(const TSharedPointer<FVulkanContext>& VulkanContext, const VkSurfaceKHR& Surface, const FVulkanAdapterRequirements& Requirements);

private:
	FRendererSettings Settings;

	TSharedPointer<FVulkanContext> VulkanContext = nullptr;
	VkSurfaceKHR Surface = VK_NULL_HANDLE;
	TSharedPointer<FVulkanAdapter> VulkanAdapter = nullptr;
	TSharedPointer<FVulkanDevice> VulkanDevice = nullptr;

	TSharedPointer<FVulkanSwapchain> Swapchain = nullptr;
	uint8 MaxFrames = 0;

	TVector<FRenderFrame> Frames;
	uint8 FrameIndex = 0;
	uint32 ImageIndex = 0;

	friend class ENGINE_API FEngine;
};
