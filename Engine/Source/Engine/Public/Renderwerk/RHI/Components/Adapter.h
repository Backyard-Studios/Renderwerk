#pragma once

#include "Renderwerk/RHI/RHICommon.h"

struct RENDERWERK_API FAdapterCapabilities
{
	EFeatureLevel FeatureLevel = EFeatureLevel::None;
	EShaderModel ShaderModel = EShaderModel::None;
	ERaytracingTier RaytracingTier = ERaytracingTier::None;
	EVariableShadingRateTier VariableShadingRateTier = EVariableShadingRateTier::None;
	EMeshShaderTier MeshShaderTier = EMeshShaderTier::None;
};

class RENDERWERK_API FAdapter : public IRHIObject
{
public:
	FAdapter(const TComPtr<IDXGIAdapter4>& InAdapter, uint32 InIndex);
	~FAdapter() override;

	DELETE_COPY_AND_MOVE(FAdapter);

public:
	NODISCARD TComPtr<IDXGIAdapter4> GetAdapter() const { return Adapter; }

	NODISCARD uint32 GetIndex() const { return Index; }
	NODISCARD FString GetName() const { return Name; }
	NODISCARD EAdapterType GetType() const { return Type; }
	NODISCARD EAdapterVendor GetVendor() const { return Vendor; }

	NODISCARD const FAdapterCapabilities& GetCapabilities() const { return Capabilities; }

private:
	TComPtr<IDXGIAdapter4> Adapter;
	uint32 Index;

	DXGI_ADAPTER_DESC3 Description;

	FString Name;
	EAdapterType Type;
	EAdapterVendor Vendor;

	FAdapterCapabilities Capabilities;
};
