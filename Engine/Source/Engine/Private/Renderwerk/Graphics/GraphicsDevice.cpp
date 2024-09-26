#include "pch.h"

#include "Renderwerk/Graphics/GraphicsDevice.h"

void InfoQueueCallback(D3D12_MESSAGE_CATEGORY Category, D3D12_MESSAGE_SEVERITY Severity, D3D12_MESSAGE_ID MessageId, LPCSTR Description, void* Context)
{
	RW_LOG_ERROR("D3D12 Message: {}", Description);
}

FGraphicsDevice::FGraphicsDevice(const FGraphicsDeviceDesc& InDescription)
	: Description(InDescription)
{
	TSharedPtr<FGraphicsAdapter>& Adapter = Description.Adapter;
	HRESULT CreateDeviceResult = D3D12CreateDevice(Adapter->GetHandle().Get(), Adapter->GetMaxSupportedFeatureLevel(), IID_PPV_ARGS(Device.GetAddressOf()));
	CHECK_RESULT(CreateDeviceResult, "Failed to create D3D12 device")

#if RW_ENABLE_D3D12_DEBUG_LAYER
	CHECK_RESULT(Device->QueryInterface(IID_PPV_ARGS(&DebugDevice)), "Failed to create D3D12 debug device")
	CHECK_RESULT(DebugDevice->QueryInterface(IID_PPV_ARGS(&InfoQueue)), "Failed to create D3D12 info queue")
	CHECK_RESULT(InfoQueue->RegisterMessageCallback(InfoQueueCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &InfoQueueCookie),
	             "Failed to register message callback")
#endif
}

FGraphicsDevice::~FGraphicsDevice()
{
#if RW_ENABLE_D3D12_DEBUG_LAYER
	if (InfoQueue)
	{
		HRESULT UnregisterResult = InfoQueue->UnregisterMessageCallback(InfoQueueCookie);
		if (FAILED(UnregisterResult))
			RW_LOG_ERROR("Failed to unregister message callback: {}", FPlatform::GetResultHandleDescription(UnregisterResult));
	}
	InfoQueue.Reset();
	DebugDevice.Reset();
#endif
	Device.Reset();
}

TSharedPtr<FCommandQueue> FGraphicsDevice::CreateCommandQueue(const FCommandQueueDesc& InDescription)
{
	return MakeShared<FCommandQueue>(this, InDescription);
}
