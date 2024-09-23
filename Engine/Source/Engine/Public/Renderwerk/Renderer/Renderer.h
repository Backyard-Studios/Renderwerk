#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Graphics/VulkanContext.h"

struct ENGINE_API FRendererDesc
{
	uint32 AdapterIndex = UINT32_MAX; // This will be set by the settings system in the future. For now it should hold an invalid value.
};

class ENGINE_API FRenderer
{
public:
	FRenderer(const FRendererDesc& InDescription);
	~FRenderer();

	DELETE_COPY_AND_MOVE(FRenderer)

public:
	[[nodiscard]] FResult Initialize();
	void Destroy();

private:
	FRendererDesc Description;

	TSharedPointer<FVulkanContext> VulkanContext;
	TSharedPointer<FVulkanAdapter> Adapter;
};
