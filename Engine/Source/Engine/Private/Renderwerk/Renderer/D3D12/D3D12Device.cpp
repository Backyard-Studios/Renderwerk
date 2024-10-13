#include "pch.h"

#include "Renderwerk/Renderer/D3D12/D3D12Device.h"

#include "Renderwerk/Renderer/D3D12/D3D12Adapter.h"

FD3D12Device::FD3D12Device(const TSharedPtr<IAdapter>& InAdapter)
	: IDevice(InAdapter)
{
	TSharedPtr<FD3D12Adapter> D3D12Adapter = static_pointer_cast<FD3D12Adapter>(Adapter);
	HRESULT CreateResult = D3D12CreateDevice(D3D12Adapter->GetHandle().Get(), D3D12Adapter->GetMaxSupportedFeatureLevel(), IID_PPV_ARGS(Device.GetAddressOf()));
	CHECK_RESULT(CreateResult, "Failed to create D3D12 device")
}

FD3D12Device::~FD3D12Device()
{
	Device.Reset();
}
