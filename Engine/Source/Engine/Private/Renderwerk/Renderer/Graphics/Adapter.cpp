#include "pch.h"

#include "Renderwerk/Renderer/Graphics/Adapter.h"

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

FString ToString(const EAdapterType& Type)
{
	switch (Type)
	{
	ENUM_CASE(EAdapterType, Software);
	ENUM_CASE(EAdapterType, Discrete);
	case EAdapterType::Unknown:
	default:
		return TEXT("Unknown");
	}
}

FString ToString(const EAdapterVendor& Vendor)
{
	switch (Vendor)
	{
	ENUM_CASE(EAdapterVendor, NVIDIA);
	ENUM_CASE(EAdapterVendor, AMD);
	ENUM_CASE(EAdapterVendor, Intel);
	ENUM_CASE(EAdapterVendor, Microsoft);
	case EAdapterVendor::Unknown:
	default:
		return TEXT("Unknown");
	}
}

FAdapter::FAdapter(const ComPtr<IDXGIAdapter4>& InAdapter, const uint32& InIndex)
	: Adapter(InAdapter), Index(InIndex)
{
	Description = {};
	DEBUG_CHECK_RESULT(Adapter->GetDesc3(&Description))

	Name = Description.Description;
	if (Description.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
		Type = EAdapterType::Software;
	else
		Type = EAdapterType::Discrete;
	Vendor = ConvertVendor(Description.VendorId);

	Capabilities = {};

	ComPtr<ID3D12Device14> TempDevice;
	HRESULT TempDeviceCreateResult = D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&TempDevice));
	CHECK_RESULTM(TempDeviceCreateResult, "Failed to create device! Device needs to support at least feature level 11.0")

	CD3DX12FeatureSupport FeatureSupport = {};
	CHECK_RESULTM(FeatureSupport.Init(TempDevice.Get()), "Failed to initialize feature support")

	Capabilities.MaxFeatureLevel = static_cast<EFeatureLevel>(FeatureSupport.MaxSupportedFeatureLevel());
	Capabilities.MaxShaderModel = static_cast<EShaderModel>(FeatureSupport.HighestShaderModel());
	Capabilities.RayTracingTier = static_cast<ERayTracingTier>(FeatureSupport.RaytracingTier());
	Capabilities.VariableShadingRateTier = static_cast<EVariableShadingRateTier>(FeatureSupport.VariableShadingRateTier());
	Capabilities.bSupportsAdditionalShadingRates = FeatureSupport.AdditionalShadingRatesSupported();
	Capabilities.MeshShaderTier = static_cast<EMeshShaderTier>(FeatureSupport.MeshShaderTier());
}

FAdapter::~FAdapter()
{
	Adapter.Reset();
}
