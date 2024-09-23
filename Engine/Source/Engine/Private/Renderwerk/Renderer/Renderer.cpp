#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Graphics/VulkanUtils.h"

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

	if (Device)
		Device->Destroy();
	Device.Reset();
	Adapter.Reset();
	vkDestroySurfaceKHR(VulkanContext->GetInstance(), Surface, VulkanContext->GetAllocator());
	if (VulkanContext)
		VulkanContext->Destroy();
	VulkanContext.Reset();
}
