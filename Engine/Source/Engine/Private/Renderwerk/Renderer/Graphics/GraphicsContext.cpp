#include "pch.h"

#include "Renderwerk/Renderer/Graphics/GraphicsContext.h"
#include "Renderwerk/Renderer/Graphics/Adapter.h"

FGraphicsContext::FGraphicsContext()
{
	RW_PROFILING_MARK_FUNCTION();

#if RW_ENABLE_GPU_DEBUGGING
	CHECK_RESULT(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DXGIDebug)));
	DXGIDebug->EnableLeakTrackingForThread();
	RW_LOG(LogGraphics, Debug, "Enabled leak tracking for current thread");

	CHECK_RESULT(D3D12GetDebugInterface(IID_PPV_ARGS(&D3D12Debug)));
	D3D12Debug->EnableDebugLayer();
	D3D12Debug->SetEnableAutoName(true);
	D3D12Debug->SetEnableGPUBasedValidation(true);
	D3D12Debug->SetEnableSynchronizedCommandQueueValidation(true);
	RW_LOG(LogGraphics, Debug, "Enabled D3D12 debug and validation layer");
#endif
	CONSTEXPR uint32 FactoryCreationFlags = bIsDebuggingEnabled ? DXGI_CREATE_FACTORY_DEBUG : 0;
	CHECK_RESULT(CreateDXGIFactory2(FactoryCreationFlags, IID_PPV_ARGS(&Factory)));
}

FGraphicsContext::~FGraphicsContext()
{
	Factory.Reset();
#if RW_ENABLE_GPU_DEBUGGING
	D3D12Debug.Reset();
	if (DXGIDebug)
	{
		OutputDebugString(TEXT("DXGI Leak Report:\n"));
		DXGI_DEBUG_RLO_FLAGS Flags = static_cast<DXGI_DEBUG_RLO_FLAGS>(DXGI_DEBUG_RLO_ALL | DXGI_DEBUG_RLO_IGNORE_INTERNAL);
		CHECK_RESULT(DXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, Flags))
	}
	DXGIDebug.Reset();
#endif
}

TVector<TSharedPtr<FAdapter>> FGraphicsContext::QueryAdapters() const
{
	RW_PROFILING_MARK_FUNCTION();

	TVector<TSharedPtr<FAdapter>> Adapters;
	ComPtr<IDXGIAdapter4> TempAdapter;
	for (uint32 Index = 0; Factory->EnumAdapterByGpuPreference(Index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&TempAdapter)) != DXGI_ERROR_NOT_FOUND; ++Index)
	{
		TSharedPtr<FAdapter> Adapter = MakeShared<FAdapter>(TempAdapter, Index);
		Adapters.push_back(Adapter);
	}
	return Adapters;
}

TSharedPtr<FAdapter> FGraphicsContext::GetAdapterByIndex(const uint32& Index) const
{
	ComPtr<IDXGIAdapter4> TempAdapter;
	HRESULT Result = Factory->EnumAdapterByGpuPreference(Index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&TempAdapter));
	CHECK_RESULTM(Result, "Failed to get adapter by index {}", Index);
	return MakeShared<FAdapter>(TempAdapter, Index);
}
