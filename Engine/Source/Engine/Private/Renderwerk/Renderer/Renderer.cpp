#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

FRenderer::FRenderer(const FRendererSettings& InSettings)
	: Settings(InSettings)
{
	GraphicsContext = MakeShared<FGraphicsContext>();
}

FRenderer::~FRenderer()
{
	GraphicsContext.Reset();
}
