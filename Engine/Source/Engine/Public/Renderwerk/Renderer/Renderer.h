#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Graphics/CommandList.h"
#include "Renderwerk/Memory/SmartPointers.h"

#include "Renderwerk/Graphics/GraphicsContext.h"
#include "Renderwerk/Graphics/GraphicsDevice.h"
#include "Renderwerk/Memory/DeletionQueue.h"

struct ENGINE_API FRendererSettings
{
	uint32 BufferCount = 3;
};

struct ENGINE_API FRenderFrame
{
	TSharedPtr<FCommandList> CommandList;
	uint32 ImageIndex = 0;
};

class ENGINE_API FRenderer
{
public:
	FRenderer(const FRendererSettings& InSettings);
	~FRenderer();

	DELETE_COPY_AND_MOVE(FRenderer)

public:
	void BeginFrame();
	void EndFrame();

private:
	void SetupAdapter();
	void SetupCommandQueues();
	void SetupRenderFrames();

private:
	FRendererSettings Settings;

	FDeletionQueue DeletionQueue;

	TSharedPtr<FGraphicsContext> GraphicsContext;
	TSharedPtr<FGraphicsAdapter> Adapter;
	TSharedPtr<FGraphicsDevice> Device;

	TSharedPtr<FCommandQueue> DirectCommandQueue;
	TSharedPtr<FCommandQueue> ComputeCommandQueue;

	uint32 FrameIndex = 0;
	TVector<FRenderFrame> RenderFrames;
};
