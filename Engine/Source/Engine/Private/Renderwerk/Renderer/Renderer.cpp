#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

FRenderer::FRenderer()
{
}

FRenderer::~FRenderer()
{
}

FResult FRenderer::Initialize()
{
	FVulkanContextDesc VulkanContextDesc;
	VulkanContextDesc.bEnableValidationLayers = true;
	VulkanContext = MakeShared<FVulkanContext>(VulkanContextDesc);

	FResult VulkanContextInitializeResult = VulkanContext->Initialize();
	CHECK_RESULT(VulkanContextInitializeResult)

	return RESULT_SUCCESS;
}

void FRenderer::Destroy()
{
	if (VulkanContext)
		VulkanContext->Destroy();
	VulkanContext.Reset();
}
