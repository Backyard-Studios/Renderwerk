#include "pch.h"

#include "Renderwerk/Renderer/D3D12/D3D12RHI.h"

#include "Renderwerk/Renderer/D3D12/D3D12Adapter.h"
#include "Renderwerk/Renderer/D3D12/D3D12Device.h"

FD3D12RHI::FD3D12RHI(const FRHIDesc& InDesc)
	: IRHI(EGraphicsAPI::DirectX12, InDesc)
{
	if (Description.bEnableDebugging)
	{
		CHECK_RESULT(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DXGIDebug)), "Failed to get DXGI debug interface")
		CHECK_RESULT(D3D12GetDebugInterface(IID_PPV_ARGS(&D3D12Debug)), "Failed to get D3D12 debug interface")

		DXGIDebug->EnableLeakTrackingForThread();

		D3D12Debug->EnableDebugLayer();
		D3D12Debug->SetEnableAutoName(true);
		RW_LOG_DEBUG("Enabled D3D12 debug layer");

		if (Description.bEnableValidation)
		{
			D3D12Debug->SetEnableGPUBasedValidation(true);
			D3D12Debug->SetEnableSynchronizedCommandQueueValidation(true);
			RW_LOG_DEBUG("Enabled D3D12 validation");
		}
	}

	uint32 FactoryFlags = Description.bEnableDebugging ? DXGI_CREATE_FACTORY_DEBUG : 0;
	CHECK_RESULT(CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&Factory)), "Failed to create DXGI factory")
}

FD3D12RHI::~FD3D12RHI()
{
	Factory.Reset();
	if (Description.bEnableDebugging)
	{
		if (DXGIDebug)
		{
			OutputDebugString(TEXT("DXGI Leak Report:\n"));
			DXGI_DEBUG_RLO_FLAGS Flags = static_cast<DXGI_DEBUG_RLO_FLAGS>(DXGI_DEBUG_RLO_ALL | DXGI_DEBUG_RLO_IGNORE_INTERNAL);
			CHECK_RESULT(DXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, Flags), "Failed to report live DXGI objects")
		}
	}
	D3D12Debug.Reset();
	DXGIDebug.Reset();
}

TVector<TSharedPtr<IAdapter>> FD3D12RHI::GetAdapters()
{
	TVector<TSharedPtr<IAdapter>> Adapters;
	ComPtr<IDXGIAdapter4> TempAdapter;
	for (uint32 AdapterIndex = 0; Factory->EnumAdapterByGpuPreference(AdapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&TempAdapter)) !=
	     DXGI_ERROR_NOT_FOUND; ++AdapterIndex)
		Adapters.push_back(MakeShared<FD3D12Adapter>(TempAdapter));
	return Adapters;
}

TSharedPtr<IAdapter> FD3D12RHI::GetPrimaryAdapter()
{
	return GetAdapters().at(0);
}

TSharedPtr<IAdapter> FD3D12RHI::GetAdapter(const std::string& Name)
{
	TVector<TSharedPtr<IAdapter>> Adapters = GetAdapters();
	for (const TSharedPtr<IAdapter>& Adapter : Adapters)
		if (Adapter->GetName() == Name)
			return Adapter;
	return nullptr;
}

TSharedPtr<IAdapter> FD3D12RHI::GetSuitableAdapter()
{
	TVector<TSharedPtr<IAdapter>> Adapters = GetAdapters();
	for (const TSharedPtr<IAdapter>& Adapter : Adapters)
	{
		if (Adapter->GetType() != EAdapterType::Discrete)
			continue;
		if (!Adapter->GetCapabilities().bSupportsMeshShading)
			continue;
		return Adapter;
	}
	return nullptr;
}

TSharedPtr<IDevice> FD3D12RHI::CreateDevice(const TSharedPtr<IAdapter>& Adapter)
{
	return MakeShared<FD3D12Device>(Adapter);
}
