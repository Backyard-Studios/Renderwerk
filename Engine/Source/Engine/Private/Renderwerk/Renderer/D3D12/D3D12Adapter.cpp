#include "pch.h"

#include "Renderwerk/Renderer/D3D12/D3D12Adapter.h"

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

FD3D12Adapter::FD3D12Adapter(const ComPtr<IDXGIAdapter4>& InAdapter)
	: IAdapter(), Adapter(InAdapter)
{
	CHECK_RESULT(Adapter->GetDesc3(&Description), "Failed to get adapter description")
	AdapterName = ToNarrow(Description.Description);

	if (Description.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
		Type = EAdapterType::Software;
	else
		Type = EAdapterType::Discrete;

	Vendor = ConvertVendor(Description.VendorId);

	D3D_FEATURE_LEVEL FeatureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	for (D3D_FEATURE_LEVEL FeatureLevel : FeatureLevels)
	{
		if (SUCCEEDED(D3D12CreateDevice(Adapter.Get(), FeatureLevel, __uuidof(ID3D12Device), nullptr)))
		{
			MaxSupportedFeatureLevel = FeatureLevel;
			break;
		}
	}
	RW_ASSERT_CRITICAL(MaxSupportedFeatureLevel != D3D_FEATURE_LEVEL_1_0_GENERIC, "Failed to find supported feature level")

	ComPtr<ID3D12Device> TempDevice;
	CHECK_RESULT(D3D12CreateDevice(Adapter.Get(), MaxSupportedFeatureLevel, IID_PPV_ARGS(&TempDevice)), "Failed to create temporary device")

	D3D12_FEATURE_DATA_SHADER_MODEL ShaderModel;
	ShaderModel.HighestShaderModel = D3D_HIGHEST_SHADER_MODEL;
	CHECK_RESULT(TempDevice->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &ShaderModel, sizeof(ShaderModel)), "Failed to check shader model support");
	MaxSupportedShaderModel = ShaderModel.HighestShaderModel;

	RW_ASSERT_CRITICAL(MaxSupportedShaderModel != D3D_SHADER_MODEL_NONE, "Failed to find supported shader model")

	D3D12_FEATURE_DATA_D3D12_OPTIONS5 Options5 = {};
	CHECK_RESULT(TempDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &Options5, sizeof(Options5)), "Failed to check D3D12 options 5");
	Capabilities.bSupportsRayTracing = Options5.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;

	D3D12_FEATURE_DATA_D3D12_OPTIONS6 Options6 = {};
	CHECK_RESULT(TempDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &Options6, sizeof(Options6)), "Failed to check D3D12 options 6");
	Capabilities.bSupportsVariableRateShading = Options6.VariableShadingRateTier != D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED;

	D3D12_FEATURE_DATA_D3D12_OPTIONS7 Options7 = {};
	CHECK_RESULT(TempDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &Options7, sizeof(Options7)), "Failed to check D3D12 options 7");
	Capabilities.bSupportsMeshShading = Options7.MeshShaderTier != D3D12_MESH_SHADER_TIER_NOT_SUPPORTED;

	TempDevice.Reset();
}

FD3D12Adapter::~FD3D12Adapter()
{
	Adapter.Reset();
}

std::string FD3D12Adapter::GetName() const
{
	return AdapterName;
}
