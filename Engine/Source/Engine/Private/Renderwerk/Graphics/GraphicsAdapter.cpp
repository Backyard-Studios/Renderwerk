#include "pch.h"

#include "Renderwerk/Graphics/GraphicsAdapter.h"

std::string ToString(const EAdapterVendor Vendor)
{
	switch (Vendor)
	{
	case EAdapterVendor::NVIDIA: return "NVIDIA";
	case EAdapterVendor::AMD: return "AMD";
	case EAdapterVendor::Intel: return "Intel";
	case EAdapterVendor::Microsoft: return "Microsoft";
	case EAdapterVendor::Unknown:
	default:
		return "Unknown";
	}
}

std::string ToString(const EAdapterType Type)
{
	switch (Type)
	{
	case EAdapterType::Software: return "Software";
	case EAdapterType::Integrated: return "Integrated";
	case EAdapterType::Discrete: return "Discrete";
	case EAdapterType::Unknown:
	default:
		return "Unknown";
	}
}

FGraphicsAdapter::FGraphicsAdapter(const ComPtr<IDXGIAdapter4>& InAdapter)
	: Adapter(InAdapter), Type(EAdapterType::Unknown)
{
	CHECK_RESULT(Adapter->GetDesc3(&Description), "Failed to get adapter description")

	if (Description.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
		Type = EAdapterType::Software;
	else if (Description.Flags & DXGI_ADAPTER_FLAG3_REMOTE)
		Type = EAdapterType::Integrated;
	else
		Type = EAdapterType::Discrete;

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

	TempDevice.Reset();
}

FGraphicsAdapter::~FGraphicsAdapter()
{
	Adapter.Reset();
}

std::string FGraphicsAdapter::GetName() const
{
	return ToNarrow(Description.Description);
}

EAdapterVendor FGraphicsAdapter::GetVendor() const
{
#define DEFINE_CASE(Vendor) case static_cast<uint32>(EAdapterVendor::Vendor): return EAdapterVendor::Vendor
	switch (Description.VendorId)
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

DXGI_QUERY_VIDEO_MEMORY_INFO FGraphicsAdapter::GetVideoMemoryInfo(const DXGI_MEMORY_SEGMENT_GROUP Group) const
{
	DXGI_QUERY_VIDEO_MEMORY_INFO VideoMemoryInfo;
	CHECK_RESULT(Adapter->QueryVideoMemoryInfo(0, Group, &VideoMemoryInfo), "Failed to query video memory info")
	return VideoMemoryInfo;
}
