#include "pch.h"

#include "Renderwerk/RHI/RHIBackend.h"
#include "Renderwerk/RHI/RHIContext.h"
#include "Renderwerk/RHI/Components/Adapter.h"

FRHIBackend::FRHIBackend()
{
	Context = MakeShared<FRHIContext>();

	TVector<TSharedPtr<FAdapter>> Adapters = Context->QueryAdapters();
	RW_LOG(LogRHI, Info, "Available Adapters:", Adapters.size());
	for (const TSharedPtr<FAdapter>& Adapter : Adapters)
	{
		RW_LOG(LogRHI, Info, "\t- Adapter{}:", Adapter->GetIndex());
		RW_LOG(LogRHI, Info, "\t\t- Name: {}", Adapter->GetName());
		RW_LOG(LogRHI, Info, "\t\t- Type: {}", ToString(Adapter->GetType()));
		RW_LOG(LogRHI, Info, "\t\t- Vendor: {}", ToString(Adapter->GetVendor()));
		RW_LOG(LogRHI, Info, "\t\t- Feature Level: {}", ToString(Adapter->GetCapabilities().FeatureLevel));
		RW_LOG(LogRHI, Info, "\t\t- Shader Model: {}", ToString(Adapter->GetCapabilities().ShaderModel));
		RW_LOG(LogRHI, Info, "\t\t- Ray Tracing: {}", ToString(Adapter->GetCapabilities().RaytracingTier));
		RW_LOG(LogRHI, Info, "\t\t- Variable Shading Rate: {}", ToString(Adapter->GetCapabilities().VariableShadingRateTier));
		RW_LOG(LogRHI, Info, "\t\t- Mesh Shader: {}", ToString(Adapter->GetCapabilities().MeshShaderTier));
	}
}

FRHIBackend::~FRHIBackend()
{
	Context.reset();
}
