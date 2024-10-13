#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Renderer/D3D12/D3D12RHI.h"

FRenderer::FRenderer(const FRendererSettings& InDescription)
	: Description(InDescription)
{
	FRHIDesc RHIDesc;
	RHIDesc.bEnableDebugging = true;
	RHIDesc.bEnableValidation = true;
	RHI = CreateRHI(RHIDesc);

	TVector<TSharedPtr<IAdapter>> Adapters = RHI->GetAdapters();
	for (const TSharedPtr<IAdapter>& Adapter : Adapters)
		RW_LOG_INFO("Adapter: {0}", Adapter->GetName());
}

FRenderer::~FRenderer()
{
	RHI.reset();
}

TSharedPtr<IRHI> FRenderer::CreateRHI(const FRHIDesc& Desc) const
{
	switch (Description.GraphicsAPI)
	{
	case EGraphicsAPI::DirectX12: return MakeShared<FD3D12RHI>(Desc);
	case EGraphicsAPI::None:
	default:
		RW_DEBUG_ASSERT(false, "Invalid graphics API")
		return nullptr;
	}
}
