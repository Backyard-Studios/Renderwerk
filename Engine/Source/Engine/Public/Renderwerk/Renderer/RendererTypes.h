#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/D3D12Include.h"

enum class ENGINE_API ECommandListType : uint8
{
	Graphics,
	Compute,
	Copy,
};

ENGINE_API INLINE std::string ToString(const ECommandListType Type)
{
	switch (Type)
	{
	case ECommandListType::Graphics: return "Graphics";
	case ECommandListType::Compute: return "Compute";
	case ECommandListType::Copy: return "Copy";
	default:
		return "Unknown";
	}
}

ENGINE_API INLINE D3D12_COMMAND_LIST_TYPE ToD3D12CommandListType(const ECommandListType Type)
{
	switch (Type)
	{
	case ECommandListType::Graphics: return D3D12_COMMAND_LIST_TYPE_DIRECT;
	case ECommandListType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
	case ECommandListType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
	default:
		return D3D12_COMMAND_LIST_TYPE_NONE;
	}
}
