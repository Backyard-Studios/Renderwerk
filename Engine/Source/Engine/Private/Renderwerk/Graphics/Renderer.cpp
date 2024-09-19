#include "pch.h"

#include "Renderwerk/Graphics/Renderer.h"

FRenderer::FRenderer(const FRendererSettings& InSettings)
	: Settings(InSettings)
{
}

FRenderer::~FRenderer() = default;

FResult FRenderer::Initialize()
{
	FVulkanContextSettings VulkanContextSettings = {};
	VulkanContextSettings.APIVersion = VK_API_VERSION_1_3;
	VulkanContextSettings.bEnableValidation = Settings.bEnableValidation;
	VulkanContextSettings.bEnableDebugging = Settings.bEnableDebugging;
	VulkanContext = MakeShared<FVulkanContext>(VulkanContextSettings);

	RW_CHECK_RESULT(VulkanContext->Initialize());

	return RW_RESULT_CODE_SUCCESS;
}

void FRenderer::Shutdown()
{
	if (VulkanContext)
		VulkanContext->Shutdown();
	VulkanContext.Reset();
}
