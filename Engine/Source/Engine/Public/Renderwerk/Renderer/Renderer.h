#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/RendererContext.h"

struct ENGINE_API FRendererSettings
{
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
};
