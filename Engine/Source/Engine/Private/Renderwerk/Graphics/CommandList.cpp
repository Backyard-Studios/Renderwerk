﻿#include "pch.h"

#include "Renderwerk/Graphics/CommandList.h"

#include "Renderwerk/Graphics/GraphicsDevice.h"

FCommandList::FCommandList(FGraphicsDevice* InDevice, const FCommandListDesc& InDescription)
	: IGraphicsDeviceChild(InDevice), Description(InDescription)
{
	ComPtr<ID3D12Device14> RawDevice = Device->GetHandle();
	CHECK_RESULT(RawDevice->CreateCommandAllocator(Description.Type, IID_PPV_ARGS(&CommandAllocator)), "Failed to create command allocator")
	CHECK_RESULT(RawDevice->CreateCommandList(0, Description.Type, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&CommandList)),
	             "Failed to create command list")
}

FCommandList::~FCommandList()
{
	CommandAllocator.Reset();
	CommandList.Reset();
}

void FCommandList::Reset() const
{
	CHECK_RESULT(CommandAllocator->Reset(), "Failed to reset command allocator")
	CHECK_RESULT(CommandList->Reset(CommandAllocator.Get(), nullptr), "Failed to reset command list")
}

void FCommandList::Close() const
{
	CHECK_RESULT(CommandList->Close(), "Failed to close command list")
}

void FCommandList::TransitionResource(const ComPtr<ID3D12Resource2>& Resource, const D3D12_RESOURCE_STATES StateBefore, const D3D12_RESOURCE_STATES StateAfter) const
{
	D3D12_RESOURCE_BARRIER ResourceBarrier = {};
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = Resource.Get();
	ResourceBarrier.Transition.StateBefore = StateBefore;
	ResourceBarrier.Transition.StateAfter = StateAfter;
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	CommandList->ResourceBarrier(1, &ResourceBarrier);
}

void FCommandList::SetRenderTargetView(const FDescriptorHandle& RenderTargetViewHandle) const
{
	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView = RenderTargetViewHandle.GetCPUHandle();
	CommandList->OMSetRenderTargets(1, &RenderTargetView, FALSE, nullptr);
}

void FCommandList::ClearRenderTargetView(const FDescriptorHandle& RenderTargetViewHandle, const FClearColor& ClearColor) const
{
	float32 ClearColorArray[4] = {ClearColor.Red, ClearColor.Green, ClearColor.Blue, ClearColor.Alpha};
	CommandList->ClearRenderTargetView(RenderTargetViewHandle.GetCPUHandle(), ClearColorArray, 0, nullptr);
}
