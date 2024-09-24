#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Graphics/VulkanCommandBuffer.h"
#include "Renderwerk/Graphics/VulkanContext.h"
#include "Renderwerk/Graphics/VulkanDevice.h"
#include "Renderwerk/Graphics/VulkanSwapchain.h"

enum class ENGINE_API EBufferingMode : uint8
{
	DoubleBuffering = 2,
	TripleBuffering = 3,
};

ENGINE_API std::string ToString(const EBufferingMode& BufferingMode);

struct ENGINE_API FRendererDesc
{
	TSharedPointer<FWindow> Window;
	uint32 AdapterIndex = UINT32_MAX; // This will be set by the settings system in the future. For now it should hold an invalid value.
	EBufferingMode BufferingMode = EBufferingMode::TripleBuffering;
};

struct ENGINE_API FRenderFrameData
{
	uint32 ImageIndex; // The acquired swapchain image index.
	VkSemaphore ImageAvailableSemaphore;
	VkSemaphore RenderFinishedSemaphore;
	VkFence InFlightFence;
	TSharedPointer<FVulkanCommandPool> CommandPool;
	TSharedPointer<FVulkanCommandBuffer> MainFrameCommandBuffer;
};

class ENGINE_API FRenderer
{
public:
	FRenderer(const FRendererDesc& InDescription);
	~FRenderer();

	DELETE_COPY_AND_MOVE(FRenderer)

public:
	[[nodiscard]] FResult Initialize();
	void Destroy();

	[[nodiscard]] FResult Resize();

	[[nodiscard]] FResult BeginFrame();
	[[nodiscard]] FResult EndFrame();

private:
	[[nodiscard]] static FResult SubmitQueue(VkQueue Queue, const FRenderFrameData& RenderFrame);

private:
	FRendererDesc Description;

	TSharedPointer<FVulkanContext> VulkanContext;

	VkSurfaceKHR Surface = VK_NULL_HANDLE;

	TSharedPointer<FVulkanAdapter> Adapter;
	TSharedPointer<FVulkanDevice> Device;
	TSharedPointer<FVulkanSwapchain> Swapchain;

	uint8 FrameIndex = 0;
	TVector<FRenderFrameData> RenderFrames;
};
