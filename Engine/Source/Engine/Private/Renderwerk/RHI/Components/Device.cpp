#include "pch.h"

#include "Renderwerk/RHI/Components/Device.h"
#include "Renderwerk/RHI/Commands/CommandQueue.h"
#include "Renderwerk/RHI/Components/Adapter.h"

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

FDevice::FDevice(FAdapter* Adapter)
	: IAdapterChild(TEXT("Device"), Adapter)
{
	IDXGIAdapter4* AdapterHandle = Adapter->GetHandle().Get();
	D3D_FEATURE_LEVEL FeatureLevel = static_cast<D3D_FEATURE_LEVEL>(Adapter->GetCapabilities().FeatureLevel);
	FD3DResult CreateResult = D3D12CreateDevice(AdapterHandle, FeatureLevel, IID_PPV_ARGS(&Device));
	D3D_CHECKM(CreateResult, "Failed to create device");
	D3D12_SET_NAME(Device, GetObjectName().c_str());

#if RW_ENABLE_GPU_DEBUGGING
	DEBUG_D3D_CHECKM(Device.As(&InfoQueue), "Failed to get info queue");
	DEBUG_D3D_CHECK(InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_MESSAGE, true));
	DEBUG_D3D_CHECK(InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, true));
	DEBUG_D3D_CHECK(InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true));
	DEBUG_D3D_CHECK(InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true));
	DEBUG_D3D_CHECK(InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true));

	HRESULT RegisterResult = InfoQueue->RegisterMessageCallback(InfoQueueCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &InfoQueueCookie);
	DEBUG_D3D_CHECKM(RegisterResult, "Failed to register message callback");
	RW_LOG(LogRHI, Debug, "Registered info queue callback");
#endif

	GraphicsQueue = CreateCommandQueue(ECommandListType::Graphics);
	RHI_SET_NAME(GraphicsQueue, TEXT("GraphicsCommandQueue"));

	ComputeQueue = CreateCommandQueue(ECommandListType::Compute);
	RHI_SET_NAME(ComputeQueue, TEXT("ComputeCommandQueue"));

	CopyQueue = CreateCommandQueue(ECommandListType::Copy);
	RHI_SET_NAME(CopyQueue, TEXT("CopyCommandQueue"));
}

FDevice::~FDevice()
{
#if RW_ENABLE_GPU_DEBUGGING
	if (InfoQueue)
	{
		DEBUG_D3D_CHECKM(InfoQueue->UnregisterMessageCallback(InfoQueueCookie), "Failed to unregister message callback");
	}
	InfoQueue.Reset();
#endif
	Device.Reset();
}

TSharedPtr<FCommandQueue> FDevice::CreateCommandQueue(ECommandListType Type)
{
	return MakeShared<FCommandQueue>(this, Type);
}

const FAdapterCapabilities& FDevice::GetCapabilities() const
{
	return GetAdapter()->GetCapabilities();
}
