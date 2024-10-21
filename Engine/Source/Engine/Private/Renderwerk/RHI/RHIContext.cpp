#include "pch.h"

#include "Renderwerk/RHI/RHIContext.h"
#include "Renderwerk/RHI/Components/Adapter.h"

FRHIContext::FRHIContext()
{
	RW_LOG(LogRHI, Info, "D3D12 SDK Version: {}", D3D12_SDK_VERSION);

#if RW_ENABLE_GPU_DEBUGGING
	RW_LOG(LogRHI, Debug, "GPU debugging enabled");

	DEBUG_D3D_CHECK(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DXGIDebug)))
	DXGIDebug->EnableLeakTrackingForThread();
	RW_LOG(LogRHI, Debug, "Enabled leak tracking for thread {}", GetPlatform()->GetCurrentThreadId());

	DEBUG_D3D_CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(&D3D12Debug)))
	D3D12Debug->EnableDebugLayer();
	D3D12Debug->SetEnableAutoName(true);
	D3D12Debug->SetEnableGPUBasedValidation(true);
	D3D12Debug->SetEnableSynchronizedCommandQueueValidation(true);
	RW_LOG(LogRHI, Debug, "Enabled debug and validation layer");
#endif
	CONSTEXPR uint32 FactoryCreationFlags = RW_GPU_DEBUGGING ? DXGI_CREATE_FACTORY_DEBUG : 0;
	const HRESULT FactoryCreateResult = CreateDXGIFactory2(FactoryCreationFlags, IID_PPV_ARGS(&Factory));
	D3D_CHECKM(FactoryCreateResult, "Failed to create DXGI factory");
}

FRHIContext::~FRHIContext()
{
	Factory.Reset();
#if RW_ENABLE_GPU_DEBUGGING
	D3D12Debug.Reset();
	if (DXGIDebug)
	{
		OutputDebugString(TEXT("DXGI Leak Report:\n"));
		DXGI_DEBUG_RLO_FLAGS Flags = static_cast<DXGI_DEBUG_RLO_FLAGS>(DXGI_DEBUG_RLO_ALL | DXGI_DEBUG_RLO_IGNORE_INTERNAL);
		DEBUG_D3D_CHECK(DXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, Flags))
	}
	DXGIDebug.Reset();
#endif
}

TVector<TSharedPtr<FAdapter>> FRHIContext::QueryAdapters() const
{
	TVector<TSharedPtr<FAdapter>> Adapters;
	TComPtr<IDXGIAdapter4> TempAdapter;
	for (uint32 Index = 0; Factory->EnumAdapterByGpuPreference(Index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&TempAdapter)) != DXGI_ERROR_NOT_FOUND; ++Index)
		Adapters.push_back(MakeShared<FAdapter>(TempAdapter, Index));
	return Adapters;
}

TSharedPtr<FAdapter> FRHIContext::GetAdapterByIndex(uint32 Index) const
{
	TComPtr<IDXGIAdapter4> TempAdapter;
	if (Factory->EnumAdapterByGpuPreference(Index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&TempAdapter)) == DXGI_ERROR_NOT_FOUND)
		return nullptr;
	return MakeShared<FAdapter>(TempAdapter, Index);
}
