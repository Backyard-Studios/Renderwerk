#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include "Renderwerk/Graphics/D3D12Include.h"

#include <string>

// https://pcisig.com/membership/member-companies
enum class ENGINE_API EAdapterVendor : uint32
{
	Unknown = 0,
	NVIDIA = 0x10DE,
	AMD = 0x1002,
	Intel = 0x8086,
	Microsoft = 0x1414,
};

enum class ENGINE_API EAdapterType : uint8
{
	Unknown = 0,
	Software,
	Remote,
	Integrated,
	Discrete,
};

ENGINE_API std::string ToString(EAdapterVendor Vendor);
ENGINE_API std::string ToString(EAdapterType Type);

class ENGINE_API FGraphicsAdapter
{
public:
	FGraphicsAdapter(const ComPtr<IDXGIAdapter4>& InAdapter);
	~FGraphicsAdapter();

	DELETE_COPY_AND_MOVE(FGraphicsAdapter)

public:
	[[nodiscard]] std::string GetName() const;
	[[nodiscard]] EAdapterVendor GetVendor() const;

	[[nodiscard]] DXGI_QUERY_VIDEO_MEMORY_INFO GetVideoMemoryInfo(DXGI_MEMORY_SEGMENT_GROUP Group = DXGI_MEMORY_SEGMENT_GROUP_LOCAL) const;

public:
	[[nodiscard]] ComPtr<IDXGIAdapter4> GetHandle() const { return Adapter; }

	[[nodiscard]] DXGI_ADAPTER_DESC3 GetDescription() const { return Description; }
	[[nodiscard]] EAdapterType GetType() const { return Type; }
	[[nodiscard]] D3D_FEATURE_LEVEL GetMaxSupportedFeatureLevel() const { return MaxSupportedFeatureLevel; }
	[[nodiscard]] D3D_SHADER_MODEL GetMaxSupportedShaderModel() const { return MaxSupportedShaderModel; }

private:
	ComPtr<IDXGIAdapter4> Adapter;

	DXGI_ADAPTER_DESC3 Description;
	EAdapterType Type;

	D3D_FEATURE_LEVEL MaxSupportedFeatureLevel = D3D_FEATURE_LEVEL_1_0_GENERIC;
	D3D_SHADER_MODEL MaxSupportedShaderModel = D3D_SHADER_MODEL_NONE;
};
