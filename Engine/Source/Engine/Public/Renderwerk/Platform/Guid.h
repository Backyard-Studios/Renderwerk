﻿#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include <format>
#include <string>

class RENDERWERK_API FGuid
{
public:
	FGuid();
	FGuid(const TVector<uint8_t>& InData);
	FGuid(const std::string& InDataString);
	~FGuid() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FGuid);

public:
	bool IsValid() const;
	std::string ToString() const;

public:
	[[nodiscard]] TVector<uint8_t> GetData() const { return Data; }

public:
	bool operator==(const FGuid& Other) const;
	bool operator!=(const FGuid& Other) const;
	bool operator<(const FGuid& Other) const;
	bool operator>(const FGuid& Other) const;

	operator std::string() const;

private:
	TVector<uint8_t> Data;
};

RENDERWERK_API FGuid NewGuid();

template <>
struct std::hash<FGuid>
{
	size_t operator()(const FGuid& Guid) const noexcept
	{
		const uint64* Data = reinterpret_cast<const uint64*>(Guid.GetData().data());
		return std::hash<uint64>{}(Data[0]) ^ std::hash<uint64>{}(Data[1]);
	}
};

template <>
struct std::formatter<FGuid> : std::formatter<std::string>
{
	template <typename FormatContext>
	auto format(const FGuid& Guid, FormatContext& ctx) const
	{
		return std::formatter<std::string>::format(Guid.ToString(), ctx);
	}
};
