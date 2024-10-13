#include "pch.h"

#include "Renderwerk/Renderer/RendererContext.h"

FRendererContext::FRendererContext(const FRendererContextDesc& InDesc)
	: Description(InDesc)
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

FRendererContext::~FRendererContext()
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

TVector<TSharedPtr<FAdapter>> FRendererContext::GetAdapters() const
{
	TVector<TSharedPtr<FAdapter>> Adapters;
	ComPtr<IDXGIAdapter4> TempAdapter;
	for (uint32 AdapterIndex = 0; Factory->EnumAdapterByGpuPreference(AdapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&TempAdapter)) !=
	     DXGI_ERROR_NOT_FOUND; ++AdapterIndex)
		Adapters.push_back(MakeShared<FAdapter>(TempAdapter));
	return Adapters;
}

TSharedPtr<FAdapter> FRendererContext::GetPrimaryAdapter() const
{
	return GetAdapters().at(0);
}

TSharedPtr<FAdapter> FRendererContext::GetAdapter(const std::string& Name) const
{
	TVector<TSharedPtr<FAdapter>> Adapters = GetAdapters();
	for (const TSharedPtr<FAdapter>& Adapter : Adapters)
		if (Adapter->GetName() == Name)
			return Adapter;
	return nullptr;
}

TSharedPtr<FAdapter> FRendererContext::GetSuitableAdapter() const
{
	TVector<TSharedPtr<FAdapter>> Adapters = GetAdapters();
	for (const TSharedPtr<FAdapter>& Adapter : Adapters)
	{
		if (Adapter->GetType() != EAdapterType::Discrete)
			continue;
		if (!Adapter->GetCapabilities().bSupportsMeshShading)
			continue;
		return Adapter;
	}
	return nullptr;
}

TSharedPtr<FDevice> FRendererContext::CreateDevice(const TSharedPtr<FAdapter>& Adapter, const FDeviceDesc& Desc)
{
	return MakeShared<FDevice>(Adapter, Desc);
}
