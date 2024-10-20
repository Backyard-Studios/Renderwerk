#include "pch.h"

#include "Renderwerk/Renderer/Graphics/Device.h"

#include "Renderwerk/Renderer/Graphics/Adapter.h"
#include "Renderwerk/Renderer/Graphics/CommandQueue.h"
#include "Renderwerk/Renderer/Graphics/Fence.h"
#include "Renderwerk/Renderer/Graphics/GraphicsContext.h"

DECLARE_LOG_CATEGORY(LogD3D12, Trace);

DEFINE_LOG_CATEGORY(LogD3D12);

#if RW_ENABLE_GPU_DEBUGGING
void InfoQueueCallback(D3D12_MESSAGE_CATEGORY Category, const D3D12_MESSAGE_SEVERITY Severity, D3D12_MESSAGE_ID Id, const LPCSTR Description, void* Context)
{
	FString Message = FStringUtils::ConvertToWideString(Description);
	if (Severity == D3D12_MESSAGE_SEVERITY_MESSAGE)
		RW_LOG(LogD3D12, Debug, "{}", Message);
	else if (Severity == D3D12_MESSAGE_SEVERITY_INFO)
		RW_LOG(LogD3D12, Info, "{}", Message);
	else if (Severity == D3D12_MESSAGE_SEVERITY_WARNING)
		RW_LOG(LogD3D12, Warn, "{}", Message);
	else if (Severity == D3D12_MESSAGE_SEVERITY_ERROR)
		RW_LOG(LogD3D12, Error, "{}", Message);
	else if (Severity == D3D12_MESSAGE_SEVERITY_CORRUPTION)
		RW_LOG(LogD3D12, Fatal, "{}", Message);
}
#endif

FDevice::FDevice(const TSharedPtr<FGraphicsContext>& InContext, const TSharedPtr<FAdapter>& InAdapter)
	: Context(InContext), Adapter(InAdapter)
{
	FAdapterCapabilities Capabilities = Adapter->GetCapabilities();
	IDXGIAdapter4* AdapterHandle = Adapter->GetHandle().Get();
	const HRESULT CreateResult = D3D12CreateDevice(AdapterHandle, static_cast<D3D_FEATURE_LEVEL>(Capabilities.MaxFeatureLevel), IID_PPV_ARGS(&Device));
	CHECK_RESULTM(CreateResult, "Failed to create device");

#if RW_ENABLE_GPU_DEBUGGING
	DEBUG_CHECK_RESULT(Device.As(&InfoQueue));
	HRESULT RegisterResult = InfoQueue->RegisterMessageCallback(InfoQueueCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &InfoQueueCookie);
	DEBUG_CHECK_RESULTM(RegisterResult, "Failed to register message callback");
#endif

	GraphicsQueue = CreateCommandQueue(ECommandListType::Graphics);
	ComputeQueue = CreateCommandQueue(ECommandListType::Compute);
	CopyQueue = CreateCommandQueue(ECommandListType::Copy);
}

FDevice::~FDevice()
{
	CopyQueue.reset();
	ComputeQueue.reset();
	GraphicsQueue.reset();
#if RW_ENABLE_GPU_DEBUGGING
	if (InfoQueue && InfoQueueCookie != 0)
	{
		HRESULT UnregisterResult = InfoQueue->UnregisterMessageCallback(InfoQueueCookie);
		DEBUG_CHECK_RESULTM(UnregisterResult, "Failed to unregister message callback");
	}
	InfoQueue.Reset();
#endif
	Device.Reset();
	Adapter.reset();
	Context.reset();
}

TSharedPtr<FCommandQueue> FDevice::CreateCommandQueue(const ECommandListType& Type) const
{
	return MakeShared<FCommandQueue>(Device, Type);
}

TSharedPtr<FFence> FDevice::CreateFence() const
{
	return MakeShared<FFence>(Device);
}
