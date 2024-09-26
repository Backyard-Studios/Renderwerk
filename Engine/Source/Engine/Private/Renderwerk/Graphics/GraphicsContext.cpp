#include "pch.h"

#include "Renderwerk/Graphics/GraphicsContext.h"

FGraphicsContext::FGraphicsContext()
{
#if RW_ENABLE_D3D12_DEBUG_LAYER
	CHECK_RESULT(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DXGIDebug)), "Failed to get DXGI debug interface")
	CHECK_RESULT(D3D12GetDebugInterface(IID_PPV_ARGS(&D3D12Debug)), "Failed to get D3D12 debug interface")

	DXGIDebug->EnableLeakTrackingForThread();
	RW_LOG_DEBUG("Enabled DXGI leak tracking for thread {}", GetCurrentThreadId());

	D3D12Debug->EnableDebugLayer();
	D3D12Debug->SetEnableGPUBasedValidation(true);
	D3D12Debug->SetEnableAutoName(true);
	D3D12Debug->SetEnableSynchronizedCommandQueueValidation(true);
	RW_LOG_DEBUG("Enabled D3D12 debug layer");

	constexpr uint32 FactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#else
	constexpr uint32 FactoryFlags = 0;
#endif

	CHECK_RESULT(CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&Factory)), "Failed to create DXGI factory")
}

FGraphicsContext::~FGraphicsContext()
{
	Factory.Reset();
#if RW_ENABLE_D3D12_DEBUG_LAYER
	if (DXGIDebug)
	{
		OutputDebugString(TEXT("DXGI Leak Report:\n"));
		CHECK_RESULT(DXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, static_cast<DXGI_DEBUG_RLO_FLAGS>(DXGI_DEBUG_RLO_ALL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)),
		             "Failed to report live DXGI objects")
		DXGIDebug.Reset();
	}
	D3D12Debug.Reset();
#endif
}

TSharedPtr<FGraphicsAdapter> FGraphicsContext::GetSuitableAdapter(const TVector<TSharedPtr<FGraphicsAdapter>>& AvailableAdapters,
                                                                  const FGraphicsAdapterRequirements& Requirements)
{
	for (const TSharedPtr<FGraphicsAdapter>& AvailableAdapter : AvailableAdapters)
	{
		if (AvailableAdapter->GetType() != Requirements.Type)
			continue;
		if (AvailableAdapter->GetMaxSupportedFeatureLevel() < Requirements.MinFeatureLevel)
			continue;
		if (AvailableAdapter->GetMaxSupportedShaderModel() < Requirements.MinShaderModel)
			continue;
		return AvailableAdapter;
	}
	return nullptr;
}

uint32 FGraphicsContext::GetAdapterCount() const
{
	uint32 AdapterCount = 0;
	ComPtr<IDXGIAdapter> TempAdapter;
	for (uint32 AdapterIndex = 0; Factory->EnumAdapterByGpuPreference(AdapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&TempAdapter)) !=
	     DXGI_ERROR_NOT_FOUND; ++AdapterIndex)
	{
		++AdapterCount;
		TempAdapter.Reset();
	}
	return AdapterCount;
}

TVector<TSharedPtr<FGraphicsAdapter>> FGraphicsContext::GetAdapters() const
{
	TVector<TSharedPtr<FGraphicsAdapter>> Adapters;
	uint32 AdapterCount = GetAdapterCount();
	for (uint32 Index = 0; Index < AdapterCount; ++Index)
	{
		ComPtr<IDXGIAdapter4> TempAdapter;
		CHECK_RESULT(Factory->EnumAdapterByGpuPreference(Index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&TempAdapter)), "Failed to get adapter")
		Adapters.push_back(MakeShared<FGraphicsAdapter>(TempAdapter));
	}
	return Adapters;
}
