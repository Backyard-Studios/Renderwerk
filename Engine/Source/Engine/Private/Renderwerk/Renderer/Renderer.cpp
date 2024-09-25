#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

FRenderer::FRenderer(const FRendererSettings& InSettings)
	: Settings(InSettings)
{
	GraphicsContext = MakeShared<FGraphicsContext>();
	Adapter = GraphicsContext->GetSuitableAdapter();

	DXGI_QUERY_VIDEO_MEMORY_INFO MemoryInfo = Adapter->GetVideoMemoryInfo(DXGI_MEMORY_SEGMENT_GROUP_LOCAL);

	RW_LOG_INFO("Selected Adapter:");
	RW_LOG_INFO("\t - Name: {}", ToNarrow(Adapter->GetDescription().Description));
	RW_LOG_INFO("\t - Vendor: {}", ToString(Adapter->GetVendor()));
	RW_LOG_INFO("\t - Memory: {} MB", MemoryInfo.Budget / 1024 / 1024);
}

FRenderer::~FRenderer()
{
	Adapter.Reset();
	GraphicsContext.Reset();
}
