#include "pch.h"

#include "Renderwerk/Renderer/RHI.h"

std::string ToString(const EGraphicsAPI GraphicsAPI)
{
	switch (GraphicsAPI)
	{
	case EGraphicsAPI::DirectX12: return "DirectX12";
	case EGraphicsAPI::None:
	default:
		return "None";
	}
}

IRHI::IRHI(const EGraphicsAPI& InGraphicsAPI, const FRHIDesc& InDescription)
	: GraphicsAPI(InGraphicsAPI), Description(InDescription)
{
}
