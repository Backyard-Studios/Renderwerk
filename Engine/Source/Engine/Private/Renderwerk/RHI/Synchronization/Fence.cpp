#include "pch.h"

#include "Renderwerk/RHI/Synchronization/Fence.h"
#include "Renderwerk/RHI/Commands/CommandQueue.h"

FFence::FFence(FDevice* InDevice)
	: IDeviceChild(TEXT("Fence"), InDevice)
{
	FD3DResult CreateResult = GetDeviceHandle()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));
	D3D_CHECKM(CreateResult, "Failed to create fence");
	D3D12_SET_NAME(Fence, GetObjectName().c_str());
}

FFence::~FFence()
{
	if (EventHandle)
		CloseHandle(EventHandle);
	Fence.Reset();
}

void FFence::SignalCPU()
{
	// No need to increment the variable value here, as the method will do it for us.
	SignalCPU(LastSignaledValue + 1);
}

void FFence::SignalCPU(const uint64 Value)
{
	FD3DResult SignalResult = Fence->Signal(Value);
	D3D_CHECKM(SignalResult, "Failed to signal fence");
	LastSignaledValue = Value;
}

void FFence::SignalGPU(const TSharedPtr<FCommandQueue>& CommandQueue)
{
	// No need to increment the variable value here, as the method will do it for us.
	SignalGPU(CommandQueue, LastSignaledValue + 1);
}

void FFence::SignalGPU(const TSharedPtr<FCommandQueue>& CommandQueue, const uint64 Value)
{
	D3D_CHECKM(CommandQueue->GetHandle()->Signal(Fence.Get(), Value), "Failed to signal fence");
	LastSignaledValue = Value;
}

void FFence::WaitOnCPU(const uint64 Timeout) const
{
	WaitOnCPU(LastSignaledValue, Timeout);
}

void FFence::WaitOnCPU(const uint64 Value, const uint64 Timeout) const
{
	if (IsCompleted(Value))
		return;
	D3D_CHECKM(Fence->SetEventOnCompletion(Value, EventHandle), "Failed to set event on completion")
	DWORD WaitResult = WaitForSingleObject(EventHandle, static_cast<DWORD>(Timeout));
	ASSERTM(WaitResult == WAIT_OBJECT_0, "Failed to wait for fence")
}

void FFence::WaitOnGPU(const TSharedPtr<FCommandQueue>& CommandQueue) const
{
	WaitOnGPU(CommandQueue, LastSignaledValue);
}

void FFence::WaitOnGPU(const TSharedPtr<FCommandQueue>& CommandQueue, const uint64 Value) const
{
	D3D_CHECKM(CommandQueue->GetHandle()->Wait(Fence.Get(), Value), "Failed to wait for fence on gpu")
}

bool8 FFence::IsCompleted(const uint64 Value) const
{
	return GetLastCompletedValue() >= Value;
}

uint64 FFence::GetLastCompletedValue() const
{
	return Fence->GetCompletedValue();
}
