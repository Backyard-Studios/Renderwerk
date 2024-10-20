#include "pch.h"

#include "Renderwerk/Renderer/Graphics/GraphicsContext.h"

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
	RW_PROFILING_MARK_FUNCTION();

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
