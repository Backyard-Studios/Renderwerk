#include "pch.h"

#include "Renderwerk/Renderer/Graphics/Fence.h"
#include "Renderwerk/Renderer/Graphics/CommandQueue.h"

FFence::FFence(const ComPtr<ID3D12Device14>& Device)
	: SignaledValue(0)
{
	FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	HRESULT CreateResult = Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));
	CHECK_RESULTM(CreateResult, "Failed to create fence")
}

FFence::~FFence()
{
	if (FenceEvent)
		CloseHandle(FenceEvent);
	Fence.Reset();
}

void FFence::Signal(const TSharedPtr<FCommandQueue>& CommandQueue)
{
	++SignaledValue;
	DEBUG_CHECK_RESULTM(CommandQueue->GetHandle()->Signal(Fence.Get(), SignaledValue), "Failed to signal fence")
}

void FFence::WaitCPU(const uint64 Timeout) const
{
	if (IsCompleted(SignaledValue))
		return;
	DEBUG_CHECK_RESULTM(Fence->SetEventOnCompletion(SignaledValue, FenceEvent), "Failed to set event on completion")
	DWORD WaitResult = WaitForSingleObject(FenceEvent, static_cast<DWORD>(Timeout));
	ASSERTM(WaitResult == WAIT_OBJECT_0, "Failed to wait for fence")
}

void FFence::WaitGPU(const TSharedPtr<FCommandQueue>& CommandQueue, const uint64 Value) const
{
	DEBUG_CHECK_RESULTM(CommandQueue->GetHandle()->Wait(Fence.Get(), Value), "Failed to wait for fence")
}

bool8 FFence::IsCompleted(const uint64 Value) const
{
	return Fence->GetCompletedValue() >= Value;
}

uint64 FFence::GetCompletedValue() const
{
	return Fence->GetCompletedValue();
}
