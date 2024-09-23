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

	return RESULT_SUCCESS;
}

void FRenderer::Destroy()
{
	Adapter.Reset();
	if (VulkanContext)
		VulkanContext->Destroy();
	VulkanContext.Reset();
}
