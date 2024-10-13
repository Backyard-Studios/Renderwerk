#pragma once

#include "RHI.h"

#include "Renderwerk/Core/CoreMinimal.h"

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
};
