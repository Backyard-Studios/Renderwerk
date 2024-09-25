#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Graphics/GraphicsContext.h"

struct ENGINE_API FRendererSettings
{
};

class ENGINE_API FRenderer
{
public:
	FRenderer(const FRendererSettings& InSettings);
	~FRenderer();

	DELETE_COPY_AND_MOVE(FRenderer)

private:
	FRendererSettings Settings;

	TSharedPointer<FGraphicsContext> GraphicsContext;
	TSharedPointer<FGraphicsAdapter> Adapter;
};
