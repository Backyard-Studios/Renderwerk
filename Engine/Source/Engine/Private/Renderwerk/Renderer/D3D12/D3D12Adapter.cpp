#include "pch.h"

#include "Renderwerk/Renderer/D3D12/D3D12Adapter.h"

EAdapterVendor ConvertVendor(const uint32 VendorId)
{
#define DEFINE_CASE(Vendor) case static_cast<uint32>(EAdapterVendor::Vendor): return EAdapterVendor::Vendor
	switch (VendorId)
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

FD3D12Adapter::FD3D12Adapter(const ComPtr<IDXGIAdapter4>& InAdapter)
	: IAdapter(), Adapter(InAdapter)
{
	CHECK_RESULT(Adapter->GetDesc3(&Description), "Failed to get adapter description")
	AdapterName = ToNarrow(Description.Description);

	if (Description.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
		Type = EAdapterType::Software;
	else
		Type = EAdapterType::Discrete;

	Vendor = ConvertVendor(Description.VendorId);
}

FD3D12Adapter::~FD3D12Adapter()
{
	Adapter.Reset();
}

std::string FD3D12Adapter::GetName() const
{
	return AdapterName;
}
