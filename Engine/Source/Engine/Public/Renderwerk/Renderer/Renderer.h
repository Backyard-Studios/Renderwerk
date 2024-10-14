#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Platform/Window.h"
#include "Renderwerk/Renderer/RendererContext.h"
#include "Renderwerk/Renderer/ShaderCompiler.h"
#include "Renderwerk/Renderer/Swapchain.h"

struct ENGINE_API FRendererSettings
{
	TSharedPtr<FWindow> Window;
};

class ENGINE_API FRenderer
{
public:
	FRenderer(const FRendererSettings& InDescription);
	~FRenderer();

	DELETE_COPY_AND_MOVE(FRenderer);

private:
	FRendererSettings Description;

	TSharedPtr<FRendererContext> Context;

	TSharedPtr<FDevice> Device;
	TSharedPtr<FSwapchain> Swapchain;

	TSharedPtr<FShaderCompiler> ShaderCompiler;
};
