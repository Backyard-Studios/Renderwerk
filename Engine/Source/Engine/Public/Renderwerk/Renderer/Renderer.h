#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Graphics/CommandList.h"
#include "Renderwerk/Memory/SmartPointers.h"

#include "Renderwerk/Graphics/GraphicsContext.h"
#include "Renderwerk/Graphics/GraphicsDevice.h"
#include "Renderwerk/Graphics/GraphicsResource.h"
#include "Renderwerk/Graphics/ShaderCompiler.h"
#include "Renderwerk/Graphics/Swapchain.h"
#include "Renderwerk/Memory/DeletionQueue.h"

struct ENGINE_API FRendererSettings
{
	uint32 BufferCount = 3;
};

struct ENGINE_API FRenderFrame
{
	TSharedPtr<FFence> Fence;
	uint32 ImageIndex = 0;
	TSharedPtr<FCommandList> CommandList;
};

class ENGINE_API FRenderer
{
public:
	FRenderer(const FRendererSettings& InSettings);
	~FRenderer();

	DELETE_COPY_AND_MOVE(FRenderer);

public:
	void BeginFrame();
	void EndFrame();

	void Resize();

private:
	void SetupAdapter();
	void SetupCommandQueues();
	void SetupRenderFrames();

	void FlushFrames();

private:
	static void Flush(const TSharedPtr<FCommandQueue>& InCommandQueue, const TSharedPtr<FFence>& InFence);

private:
	FRendererSettings Settings;

	FDeletionQueue DeletionQueue;

	TSharedPtr<FGraphicsContext> GraphicsContext;
	TSharedPtr<FGraphicsAdapter> Adapter;
	TSharedPtr<FGraphicsDevice> Device;

	TSharedPtr<FCommandQueue> DirectCommandQueue;
	TSharedPtr<FCommandQueue> ComputeCommandQueue;

	TSharedPtr<FDescriptorHeap> RenderTargetViewHeap;

	TSharedPtr<FSwapchain> Swapchain;

	uint32 FrameIndex = 0;
	TVector<FRenderFrame> RenderFrames;

	TSharedPtr<FShaderCompiler> ShaderCompiler;

	ComPtr<ID3D12RootSignature> RootSignature;
	ComPtr<ID3D12PipelineState> PipelineState;
	TSharedPtr<FGraphicsBuffer> VertexBuffer;
	TSharedPtr<FGraphicsBuffer> IndexBuffer;
};
