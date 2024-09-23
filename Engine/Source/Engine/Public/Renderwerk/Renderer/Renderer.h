#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Graphics/VulkanContext.h"

class ENGINE_API FRenderer
{
public:
	FRenderer();
	~FRenderer();

	DELETE_COPY_AND_MOVE(FRenderer)

public:
	[[nodiscard]] FResult Initialize();
	void Destroy();

private:
	TSharedPointer<FVulkanContext> VulkanContext;
};
