#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/DataTypes/Types.h"
#include "Renderwerk/Renderer/Graphics/GraphicsCommon.h"

class RENDERWERK_API FGraphicsContext
{
public:
	FGraphicsContext();
	~FGraphicsContext();

	DELETE_COPY_AND_MOVE(FGraphicsContext);

public:
	NODISCARD ComPtr<IDXGIFactory7> GetFactory() const { return Factory; }

public:
	static NODISCARD CONSTEXPR bool8 IsDebuggingEnabled() { return bIsDebuggingEnabled; }

private:
#if RW_ENABLE_GPU_DEBUGGING
	CONSTEXPR static bool8 bIsDebuggingEnabled = true;

	ComPtr<IDXGIDebug1> DXGIDebug;
	ComPtr<ID3D12Debug6> D3D12Debug;
#else
	
	CONSTEXPR static bool8 bIsDebuggingEnabled = false;
#endif

	ComPtr<IDXGIFactory7> Factory;
};
