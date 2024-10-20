#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/Graphics/GraphicsCommon.h"

enum class RENDERWERK_API EAdapterType : uint8
{
	Unknown = 0,
	Software,
	Discrete,
};

RENDERWERK_API FString ToString(const EAdapterType& Type);

// https://pcisig.com/membership/member-companies
enum class RENDERWERK_API EAdapterVendor : uint16
{
	Unknown = 0,
	NVIDIA = 0x10DE,
	AMD = 0x1002,
	Intel = 0x8086,
	Microsoft = 0x1414,
};

RENDERWERK_API FString ToString(const EAdapterVendor& Vendor);

struct RENDERWERK_API FAdapterCapabilities
{
	EFeatureLevel MaxFeatureLevel = EFeatureLevel::None;
	EShaderModel MaxShaderModel = EShaderModel::None;
	ERayTracingTier RayTracingTier = ERayTracingTier::None;
	EVariableShadingRateTier VariableShadingRateTier = EVariableShadingRateTier::None;
	bool8 bSupportsAdditionalShadingRates = false;
	EMeshShaderTier MeshShaderTier = EMeshShaderTier::None;
};

class RENDERWERK_API FAdapter
{
public:
	FAdapter(const ComPtr<IDXGIAdapter4>& InAdapter, const uint32& InIndex);
	~FAdapter();

	DELETE_COPY_AND_MOVE(FAdapter);

public:
	NODISCARD ComPtr<IDXGIAdapter4> GetHandle() const { return Adapter; }

	NODISCARD uint32 GetIndex() const { return Index; }

	NODISCARD FString GetName() const { return Name; }
	NODISCARD EAdapterType GetType() const { return Type; }
	NODISCARD EAdapterVendor GetVendor() const { return Vendor; }
	NODISCARD const FAdapterCapabilities& GetCapabilities() const { return Capabilities; }

private:
	ComPtr<IDXGIAdapter4> Adapter;
	uint32 Index;

	DXGI_ADAPTER_DESC3 Description;

	FString Name;
	EAdapterType Type;
	EAdapterVendor Vendor;

	FAdapterCapabilities Capabilities;
};
