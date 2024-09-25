#include "pch.h"

#include "Renderwerk/Graphics/GraphicsAdapter.h"

std::string ToString(const EAdapterVendor Vendor)
{
	switch (Vendor)
	{
	case EAdapterVendor::NVIDIA: return "NVIDIA";
	case EAdapterVendor::AMD: return "AMD";
	case EAdapterVendor::Intel: return "Intel";
	case EAdapterVendor::Unknown:
	default:
		return "Unknown";
	}
}

FGraphicsAdapter::FGraphicsAdapter(const ComPtr<IDXGIAdapter4>& InAdapter)
	: Adapter(InAdapter)
{
	CHECK_RESULT(Adapter->GetDesc3(&Description), "Failed to get adapter description")
}

FGraphicsAdapter::~FGraphicsAdapter()
{
	Adapter.Reset();
}

EAdapterVendor FGraphicsAdapter::GetVendor() const
{
#define DEFINE_CASE(Vendor) case static_cast<uint32>(EAdapterVendor::Vendor): return EAdapterVendor::Vendor
	switch (Description.VendorId)
	{
	DEFINE_CASE(NVIDIA);
	DEFINE_CASE(AMD);
	DEFINE_CASE(Intel);
	default:
		return EAdapterVendor::Unknown;
	}
#undef DEFINE_CASE
}

bool8 FGraphicsAdapter::IsSoftwareBased() const
{
	return Description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE;
}

bool8 FGraphicsAdapter::IsHardwareBased() const
{
	return !IsSoftwareBased();
}

DXGI_QUERY_VIDEO_MEMORY_INFO FGraphicsAdapter::GetVideoMemoryInfo(const DXGI_MEMORY_SEGMENT_GROUP Group) const
{
	DXGI_QUERY_VIDEO_MEMORY_INFO VideoMemoryInfo;
	CHECK_RESULT(Adapter->QueryVideoMemoryInfo(0, Group, &VideoMemoryInfo), "Failed to query video memory info")
	return VideoMemoryInfo;
}
