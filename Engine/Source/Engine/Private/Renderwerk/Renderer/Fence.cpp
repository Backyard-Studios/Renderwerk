#include "pch.h"

#include "Renderwerk/Renderer/Fence.h"

FFence::FFence(const ComPtr<ID3D12Device14>& Device)
	: LastSignaledValue(0)
{
	CHECK_RESULT(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)), "Failed to create fence")
	FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

FFence::~FFence()
{
	if (FenceEvent)
		CloseHandle(FenceEvent);
	Fence.Reset();
}

void FFence::Signal(const ComPtr<ID3D12CommandQueue>& CommandQueue)
{
	++LastSignaledValue;
	CHECK_RESULT(CommandQueue->Signal(Fence.Get(), LastSignaledValue), "Failed to signal fence");
}

void FFence::WaitCPU(const uint64 Timeout) const
{
	if (IsValueComplete(LastSignaledValue))
		return;
	CHECK_RESULT(Fence->SetEventOnCompletion(LastSignaledValue, FenceEvent), "Failed to set event on completion")
	DWORD WaitResult = WaitForSingleObject(FenceEvent, static_cast<DWORD>(Timeout));
	RW_DEBUG_ASSERT(WaitResult == WAIT_OBJECT_0, "Failed to wait for fence");
}

void FFence::WaitGPU(const ComPtr<ID3D12CommandQueue>& CommandQueue, const uint64 Value) const
{
	CHECK_RESULT(CommandQueue->Wait(Fence.Get(), Value), "Failed to wait for fence")
}

uint64 FFence::GetLastCompletedValue() const
{
	return Fence->GetCompletedValue();
}

bool8 FFence::IsValueComplete(const uint64 InValue) const
{
	return Fence->GetCompletedValue() >= InValue;
}
