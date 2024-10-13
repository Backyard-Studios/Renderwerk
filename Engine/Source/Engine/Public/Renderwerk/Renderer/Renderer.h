#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/RHI.h"

struct ENGINE_API FRendererSettings
{
	EGraphicsAPI GraphicsAPI;
};

class ENGINE_API FRenderer
{
public:
	FRenderer(const FRendererSettings& InDescription);
	~FRenderer();

	DELETE_COPY_AND_MOVE(FRenderer);

private:
	TSharedPtr<IRHI> CreateRHI(const FRHIDesc& Desc) const;

private:
	FRendererSettings Description;

	TSharedPtr<IRHI> RHI;

	TSharedPtr<IDevice> Device;
};
