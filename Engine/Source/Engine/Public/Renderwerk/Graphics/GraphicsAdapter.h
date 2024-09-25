#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include "Renderwerk/Graphics/D3D12Include.h"

#include <string>

enum class ENGINE_API EAdapterVendor : uint32
{
	Unknown = 0,
	NVIDIA = 0x10DE,
	AMD = 0x1002,
	Intel = 0x8086,
};

ENGINE_API std::string ToString(EAdapterVendor Vendor);

class ENGINE_API FGraphicsAdapter
{
public:
	FGraphicsAdapter(const ComPtr<IDXGIAdapter4>& InAdapter);
	~FGraphicsAdapter();

	DELETE_COPY_AND_MOVE(FGraphicsAdapter)

public:
	[[nodiscard]] EAdapterVendor GetVendor() const;
	[[nodiscard]] bool8 IsSoftwareBased() const;
	[[nodiscard]] bool8 IsHardwareBased() const;

	[[nodiscard]] DXGI_QUERY_VIDEO_MEMORY_INFO GetVideoMemoryInfo(DXGI_MEMORY_SEGMENT_GROUP Group = DXGI_MEMORY_SEGMENT_GROUP_LOCAL) const;

public:
	[[nodiscard]] ComPtr<IDXGIAdapter4> GetHandle() const { return Adapter; }

	[[nodiscard]] DXGI_ADAPTER_DESC3 GetDescription() const { return Description; }

private:
	ComPtr<IDXGIAdapter4> Adapter;
	DXGI_ADAPTER_DESC3 Description;
};
