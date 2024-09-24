#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Graphics/VulkanUtils.h"

std::string ToString(const EBufferingMode& BufferingMode)
{
	switch (BufferingMode)
	{
	case EBufferingMode::DoubleBuffering: return "DoubleBuffering";
	case EBufferingMode::TripleBuffering: return "TripleBuffering";
	default:
		return "Unknown";
	}
}

FRenderer::FRenderer(const FRendererDesc& InDescription)
	: Description(InDescription)
{
}

FRenderer::~FRenderer()
{
}

FResult FRenderer::Initialize()
{
	FVulkanContextDesc VulkanContextDesc;
	VulkanContextDesc.bEnableValidationLayers = true;
	VulkanContextDesc.AdapterIndex = Description.AdapterIndex;
	VulkanContext = MakeShared<FVulkanContext>(VulkanContextDesc);

	FResult VulkanContextInitializeResult = VulkanContext->Initialize();
	CHECK_RESULT(VulkanContextInitializeResult)

	CHECK_RESULT(VulkanContext->CreateSurface(Description.Window, &Surface));

	if (Description.AdapterIndex == UINT32_MAX)
		Adapter = VulkanContext->GetSuitableAdapter();
	else
		Adapter = VulkanContext->GetAdapterByIndex(Description.AdapterIndex);
	CHECK_RESULT(Adapter->Initialize());

	RW_LOG_INFO("Selected adapter:");
	RW_LOG_INFO("\t- Name: {}", Adapter->GetProperties().deviceName);
	RW_LOG_INFO("\t- Vendor: {}", ToString(Adapter->GetVendor()));
	RW_LOG_INFO("\t- Type: {}", ToString(Adapter->GetProperties().deviceType));
	RW_LOG_INFO("\t- API Version: {}", VulkanFormatVersion(Adapter->GetProperties().apiVersion));

	Device = MakeShared<FVulkanDevice>(FVulkanDeviceDesc{VulkanContext, Adapter, Surface});
	CHECK_RESULT(Device->Initialize());

	VmaVulkanFunctions VulkanFunctions = {};
	VulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
	VulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

	ResourceAllocator = MakeShared<FVulkanResourceAllocator>();
	CHECK_RESULT(ResourceAllocator->Initialize())

	FVulkanSwapchainDesc SwapchainDesc = {};
	SwapchainDesc.Context = VulkanContext;
	SwapchainDesc.Device = Device;
	SwapchainDesc.Window = Description.Window;
	SwapchainDesc.Surface = Surface;
	CHECK_RESULT(Adapter->QuerySurfaceProperties(Surface, &SwapchainDesc.SurfaceProperties))
	Swapchain = MakeShared<FVulkanSwapchain>(SwapchainDesc);
	CHECK_RESULT(Swapchain->Initialize());

	RenderFrames.resize(static_cast<uint8>(Description.BufferingMode));
	for (FRenderFrameData& RenderFrame : RenderFrames)
	{
		RenderFrame.ImageIndex = UINT32_MAX;
		CHECK_RESULT(Device->CreateVulkanSemaphore(0, &RenderFrame.ImageAvailableSemaphore))
		CHECK_RESULT(Device->CreateVulkanSemaphore(0, &RenderFrame.RenderFinishedSemaphore))
		CHECK_RESULT(Device->CreateFence(VK_FENCE_CREATE_SIGNALED_BIT, &RenderFrame.InFlightFence))

		FVulkanCommandPoolDesc CommandPoolDesc = {};
		CommandPoolDesc.Context = VulkanContext;
		CommandPoolDesc.Device = Device;
		CommandPoolDesc.QueueIndex = Device->GetQueueData().GraphicsIndex;
		RenderFrame.CommandPool = MakeShared<FVulkanCommandPool>(CommandPoolDesc);
		CHECK_RESULT(RenderFrame.CommandPool->Initialize())

		CHECK_RESULT(RenderFrame.CommandPool->AllocateCommandBuffer(&RenderFrame.MainFrameCommandBuffer));
	}

	return RESULT_SUCCESS;
}

void FRenderer::Destroy()
{
	if (Device)
	{
		FResult WaitResult = Device->WaitForIdle();
		if (WaitResult.IsError()) [[unlikely]]
			RW_LOG_ERROR("Failed to wait for Vulkan device to become idle");
	}

	for (FRenderFrameData& RenderFrame : RenderFrames)
	{
		if (RenderFrame.CommandPool && RenderFrame.MainFrameCommandBuffer)
			RenderFrame.CommandPool->FreeCommandBuffer(RenderFrame.MainFrameCommandBuffer);
		RenderFrame.MainFrameCommandBuffer.Reset();
		if (RenderFrame.CommandPool)
			RenderFrame.CommandPool->Destroy();
		RenderFrame.CommandPool.Reset();
		vkDestroyFence(Device->GetHandle(), RenderFrame.InFlightFence, VulkanContext->GetAllocator());
		vkDestroySemaphore(Device->GetHandle(), RenderFrame.RenderFinishedSemaphore, VulkanContext->GetAllocator());
		vkDestroySemaphore(Device->GetHandle(), RenderFrame.ImageAvailableSemaphore, VulkanContext->GetAllocator());
	}
	RenderFrames.clear();
	if (Swapchain)
		Swapchain->Destroy();
	Swapchain.Reset();
	if (ResourceAllocator)
		ResourceAllocator->Destroy();
	ResourceAllocator.Reset();
	if (Device)
		Device->Destroy();
	Device.Reset();
	Adapter.Reset();
	vkDestroySurfaceKHR(VulkanContext->GetInstance(), Surface, VulkanContext->GetAllocator());
	if (VulkanContext)
		VulkanContext->Destroy();
	VulkanContext.Reset();
}

FResult FRenderer::Resize()
{
	RW_PROFILING_MARK_FUNCTION();

	CHECK_RESULT(Device->WaitForIdle());
	CHECK_RESULT(Swapchain->Resize());
	return RESULT_SUCCESS;
}

FResult FRenderer::BeginFrame()
{
	RW_PROFILING_MARK_FUNCTION();

	vmaSetCurrentFrameIndex(ResourceAllocator->GetHandle(), FrameIndex);
	FRenderFrameData& FrameData = RenderFrames.at(FrameIndex);
	TSharedPointer<FVulkanCommandBuffer> CommandBuffer = FrameData.MainFrameCommandBuffer;

	CHECK_RESULT(Device->WaitForFence(FrameData.InFlightFence));

	CHECK_RESULT(Swapchain->AcquireNextImageIndex(FrameData.ImageAvailableSemaphore, &FrameData.ImageIndex));

	CHECK_RESULT(CommandBuffer->Reset())
	CHECK_RESULT(CommandBuffer->Begin())

	CommandBuffer->TransitionSwapchainImage(Swapchain->GetImage(FrameData.ImageIndex), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	CommandBuffer->ClearImage(Swapchain->GetImage(FrameData.ImageIndex));

	return RESULT_SUCCESS;
}

FResult FRenderer::EndFrame()
{
	RW_PROFILING_MARK_FUNCTION();

	FRenderFrameData& FrameData = RenderFrames.at(FrameIndex);
	TSharedPointer<FVulkanCommandBuffer> CommandBuffer = FrameData.MainFrameCommandBuffer;

	CommandBuffer->TransitionSwapchainImage(Swapchain->GetImage(FrameData.ImageIndex), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	CHECK_RESULT(CommandBuffer->End())

	CHECK_RESULT(SubmitQueue(Device->GetGraphicsQueue(), FrameData))

	CHECK_RESULT(Swapchain->Present(FrameData))

	FrameIndex = (FrameIndex + 1) % static_cast<uint8>(Description.BufferingMode);
	return RESULT_SUCCESS;
}

FResult FRenderer::SubmitQueue(const VkQueue Queue, const FRenderFrameData& RenderFrame)
{
	RW_PROFILING_MARK_FUNCTION();

	VkCommandBufferSubmitInfo CommandBufferSubmitInfo = {};
	CommandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	CommandBufferSubmitInfo.commandBuffer = RenderFrame.MainFrameCommandBuffer->GetHandle();

	VkSemaphoreSubmitInfo ImageSemaphoreSubmitInfo = {};
	ImageSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	ImageSemaphoreSubmitInfo.semaphore = RenderFrame.ImageAvailableSemaphore;
	ImageSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR;
	ImageSemaphoreSubmitInfo.value = 1;

	VkSemaphoreSubmitInfo RenderSemaphoreSubmitInfo = {};
	RenderSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	RenderSemaphoreSubmitInfo.semaphore = RenderFrame.RenderFinishedSemaphore;
	RenderSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
	RenderSemaphoreSubmitInfo.value = 1;

	VkSubmitInfo2 SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	SubmitInfo.commandBufferInfoCount = 1;
	SubmitInfo.pCommandBufferInfos = &CommandBufferSubmitInfo;
	SubmitInfo.waitSemaphoreInfoCount = 1;
	SubmitInfo.pWaitSemaphoreInfos = &ImageSemaphoreSubmitInfo;
	SubmitInfo.signalSemaphoreInfoCount = 1;
	SubmitInfo.pSignalSemaphoreInfos = &RenderSemaphoreSubmitInfo;

	CHECK_VKRESULT(vkQueueSubmit2(Queue, 1, &SubmitInfo, RenderFrame.InFlightFence), "Failed to submit command buffer")

	return RESULT_SUCCESS;
}

TSharedPointer<FRenderer> GetRenderer()
{
	return GetEngine()->GetRenderer();
}
