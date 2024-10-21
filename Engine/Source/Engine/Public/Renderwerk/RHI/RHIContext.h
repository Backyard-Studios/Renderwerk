#pragma once

#include "Renderwerk/RHI/RHICommon.h"

class RENDERWERK_API FRHIContext
{
public:
	FRHIContext();
	~FRHIContext();

	DELETE_COPY_AND_MOVE(FRHIContext);

public:
	NODISCARD ComPtr<IDXGIFactory7> GetFactory() const { return Factory; }

private:
#if RW_ENABLE_GPU_DEBUGGING
	ComPtr<IDXGIDebug1> DXGIDebug;
	ComPtr<ID3D12Debug6> D3D12Debug;
#endif

	ComPtr<IDXGIFactory7> Factory;
};
