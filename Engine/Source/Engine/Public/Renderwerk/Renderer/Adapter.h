#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

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

class ENGINE_API IAdapter
{
public:
	IAdapter() = default;
	virtual ~IAdapter() = default;

	DELETE_COPY_AND_MOVE(IAdapter);

public:
	[[nodiscard]] virtual std::string GetName() const = 0;

public:
	[[nodiscard]] EAdapterType GetType() const { return Type; }
	[[nodiscard]] EAdapterVendor GetVendor() const { return Vendor; }

	[[nodiscard]] FAdapterCapabilities GetCapabilities() const { return Capabilities; }

protected:
	EAdapterType Type = EAdapterType::Unknown;
	EAdapterVendor Vendor = EAdapterVendor::Unknown;

	FAdapterCapabilities Capabilities;
};
