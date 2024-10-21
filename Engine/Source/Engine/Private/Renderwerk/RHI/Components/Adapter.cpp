#include "pch.h"

#include "Renderwerk/RHI/Components/Adapter.h"

EAdapterVendor ConvertVendor(const uint32 VendorId)
{
#define DEFINE_CASE(Vendor) case static_cast<uint32>(EAdapterVendor::Vendor): return EAdapterVendor::Vendor
	switch (VendorId)
	{
	DEFINE_CASE(NVIDIA);
	DEFINE_CASE(AMD);
	DEFINE_CASE(Intel);
	DEFINE_CASE(Microsoft);
	default:
		return EAdapterVendor::Unknown;
	}
#undef DEFINE_CASE
}

FAdapter::FAdapter(const TComPtr<IDXGIAdapter4>& InAdapter, const uint32 InIndex)
	: IRHIObject(TEXT("Adapter")), Adapter(InAdapter), Index(InIndex)
{
	DEBUG_ASSERTM(Adapter, "Adapter is null");

	DEBUG_D3D_CHECK(Adapter->GetDesc3(&Description));

	Name = Description.Description;
	if (Description.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
		Type = EAdapterType::Software;
	else
		Type = EAdapterType::Discrete;
	Vendor = ConvertVendor(Description.VendorId);

	TComPtr<ID3D12Device> TempDevice;
	FD3DResult CreateResult = D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&TempDevice));
	D3D_CHECKM(CreateResult, "Failed to create device! Device doesn't support feature level 11.0");

	CD3DX12FeatureSupport FeatureSupport;
	FD3DResult FeatureInitResult = FeatureSupport.Init(TempDevice.Get());
	D3D_CHECKM(FeatureInitResult, "Failed to initialize feature support!");

	Capabilities.FeatureLevel = static_cast<EFeatureLevel>(FeatureSupport.MaxSupportedFeatureLevel());
	Capabilities.ShaderModel = static_cast<EShaderModel>(FeatureSupport.HighestShaderModel());
	Capabilities.RaytracingTier = static_cast<ERaytracingTier>(FeatureSupport.RaytracingTier());
	Capabilities.VariableShadingRateTier = static_cast<EVariableShadingRateTier>(FeatureSupport.VariableShadingRateTier());
	Capabilities.MeshShaderTier = static_cast<EMeshShaderTier>(FeatureSupport.MeshShaderTier());
}

FAdapter::~FAdapter()
{
	Adapter.Reset();
}
