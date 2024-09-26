#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Engine/Engine.h"

FRenderer::FRenderer(const FRendererSettings& InSettings)
	: Settings(InSettings)
{
	GraphicsContext = MakeShared<FGraphicsContext>();

	TVector<TSharedPtr<FGraphicsAdapter>> Adapters = GraphicsContext->GetAdapters();

	RW_LOG_INFO("Available Adapters:");
	for (TSharedPtr<FGraphicsAdapter>& AvailableAdapter : Adapters)
		RW_LOG_INFO("\t - {} [Vendor: {}, Type: {}]", AvailableAdapter->GetName(), ToString(AvailableAdapter->GetVendor()), ToString(AvailableAdapter->GetType()));

	Adapter = FGraphicsContext::GetSuitableAdapter(Adapters);
	RW_LOG_INFO("Selected Adapter: {} [{}, {}]", Adapter->GetName(), ToString(Adapter->GetMaxSupportedFeatureLevel()), ToString(Adapter->GetMaxSupportedShaderModel()));

	GetEngine()->GetMainWindow()->AppendTitle(std::format(" | D3D12 <{}>", ToString(Adapter->GetMaxSupportedShaderModel())));
}

FRenderer::~FRenderer()
{
	Adapter.reset();
	GraphicsContext.reset();
}
