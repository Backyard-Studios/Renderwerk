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
	TSharedPtr<FAdapter> SelectedAdapter = SelectSuitableAdapter(Adapters);
	RW_LOG(LogRHI, Info, "Selected Adapter: Adapter{} ({})", SelectedAdapter->GetIndex(), SelectedAdapter->GetName());

	Device = SelectedAdapter->CreateDevice();
}

FRHIBackend::~FRHIBackend()
{
	Device.reset();
	Context.reset();
}

TSharedPtr<FAdapter> FRHIBackend::SelectSuitableAdapter(const TVector<TSharedPtr<FAdapter>>& Adapters)
{
	TSharedPtr<FAdapter> SelectedAdapter;
	for (const TSharedPtr<FAdapter>& Adapter : Adapters)
	{
		if (IsAdapterSuitable(Adapter))
			SelectedAdapter = Adapter;
	}
	ASSERTM(SelectedAdapter, "No suitable adapter found");
	return SelectedAdapter;
}

bool8 FRHIBackend::IsAdapterSuitable(const TSharedPtr<FAdapter>& Adapter)
{
	if (Adapter->GetType() != EAdapterType::Discrete)
		return false;
	if (Adapter->GetCapabilities().FeatureLevel < EFeatureLevel::FL_12_2)
		return false;
	if (Adapter->GetCapabilities().ShaderModel < EShaderModel::SM_6_8)
		return false;
	if (Adapter->GetCapabilities().RaytracingTier < ERaytracingTier::Tier_1_0)
		return false;
	if (Adapter->GetCapabilities().VariableShadingRateTier < EVariableShadingRateTier::Tier_1)
		return false;
	if (Adapter->GetCapabilities().MeshShaderTier < EMeshShaderTier::Tier_1)
		return false;
	return true;
}
