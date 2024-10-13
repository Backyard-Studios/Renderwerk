#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

enum class ENGINE_API EAdapterType : uint8
{
	Unknown,
	Software,
	Discrete,
};

ENGINE_API std::string ToString(EAdapterType Type);

// https://pcisig.com/membership/member-companies
enum class ENGINE_API EAdapterVendor : uint16
{
	Unknown = 0,
	NVIDIA = 0x10DE,
	AMD = 0x1002,
	Intel = 0x8086,
	Microsoft = 0x1414,
};

ENGINE_API std::string ToString(EAdapterVendor Vendor);

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

protected:
	EAdapterType Type = EAdapterType::Unknown;
	EAdapterVendor Vendor = EAdapterVendor::Unknown;
};
