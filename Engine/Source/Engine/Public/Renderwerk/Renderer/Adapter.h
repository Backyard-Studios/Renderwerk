#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/D3D12Include.h"

enum class ENGINE_API EAdapterType : uint8
{
	Unknown,
	Software,
	Discrete,
};

ENGINE_API INLINE std::string ToString(const EAdapterType Type)
{
	switch (Type)
	{
	case EAdapterType::Software: return "Software";
	case EAdapterType::Discrete: return "Discrete";
	case EAdapterType::Unknown:
	default:
		return "Unknown";
	}
}

// https://pcisig.com/membership/member-companies
enum class ENGINE_API EAdapterVendor : uint16
{
	Unknown = 0,
	NVIDIA = 0x10DE,
	AMD = 0x1002,
	Intel = 0x8086,
	Microsoft = 0x1414,
};

ENGINE_API INLINE std::string ToString(const EAdapterVendor Vendor)
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

struct ENGINE_API FAdapterCapabilities
{
	bool8 bSupportsMeshShading = false;
	bool8 bSupportsRayTracing = false;
	bool8 bSupportsVariableRateShading = false;
};

class ENGINE_API FAdapter
{
public:
	FAdapter(const ComPtr<IDXGIAdapter4>& InAdapter);
	~FAdapter();

	DELETE_COPY_AND_MOVE(FAdapter);

public:
	[[nodiscard]] std::string GetName() const;

public:
	[[nodiscard]] ComPtr<IDXGIAdapter4> GetHandle() const { return Adapter; }

	[[nodiscard]] EAdapterType GetType() const { return Type; }
	[[nodiscard]] EAdapterVendor GetVendor() const { return Vendor; }

	[[nodiscard]] D3D_FEATURE_LEVEL GetMaxSupportedFeatureLevel() const { return MaxSupportedFeatureLevel; }
	[[nodiscard]] D3D_SHADER_MODEL GetMaxSupportedShaderModel() const { return MaxSupportedShaderModel; }

	[[nodiscard]] FAdapterCapabilities GetCapabilities() const { return Capabilities; }

protected:
	EAdapterType Type = EAdapterType::Unknown;
	EAdapterVendor Vendor = EAdapterVendor::Unknown;

	ComPtr<IDXGIAdapter4> Adapter;

	DXGI_ADAPTER_DESC3 Description;
	std::string AdapterName;

	D3D_FEATURE_LEVEL MaxSupportedFeatureLevel = D3D_FEATURE_LEVEL_1_0_GENERIC;
	D3D_SHADER_MODEL MaxSupportedShaderModel = D3D_SHADER_MODEL_NONE;

	FAdapterCapabilities Capabilities;
};
