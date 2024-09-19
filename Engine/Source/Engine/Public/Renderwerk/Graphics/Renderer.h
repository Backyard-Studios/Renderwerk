#pragma once

#include "VulkanContext.h"

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Platform/Window.h"

#define RW_RESULT_CODE_VULKAN_RESULT_FAILED RW_MAKE_RESULT_CODE(RW_RESULT_SEVERITY_ERROR, 101)

struct ENGINE_API FRendererSettings
{
	TSharedPointer<IWindow> Window;
	bool bEnableValidation = false;
	bool bEnableDebugging = false;
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

private:
	FRendererSettings Settings;

	TSharedPointer<FVulkanContext> VulkanContext = nullptr;

	friend class ENGINE_API FEngine;
};
