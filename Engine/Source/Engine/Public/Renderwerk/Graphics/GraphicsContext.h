﻿#pragma once

#include "GraphicsAdapter.h"

#include "Renderwerk/Core/CoreDefinitions.h"

#include "Renderwerk/Graphics/D3D12Include.h"

class ENGINE_API FGraphicsContext
{
public:
	FGraphicsContext();
	~FGraphicsContext();

	DELETE_COPY_AND_MOVE(FGraphicsContext)

public:
	TSharedPointer<FGraphicsAdapter> GetSuitableAdapter() const;

private:
#if RW_ENABLE_D3D12_DEBUG_LAYER
	ComPtr<IDXGIDebug1> DXGIDebug;
	ComPtr<ID3D12Debug6> D3D12Debug;
#endif

	ComPtr<IDXGIFactory6> Factory;
};
