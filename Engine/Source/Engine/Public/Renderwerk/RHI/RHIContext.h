#pragma once

#include "Renderwerk/RHI/RHICommon.h"

class RENDERWERK_API FRHIContext
{
public:
	FRHIContext();
	~FRHIContext();

	DELETE_COPY_AND_MOVE(FRHIContext);

public:
	NODISCARD TComPtr<IDXGIFactory7> GetFactory() const { return Factory; }

private:
#if RW_ENABLE_GPU_DEBUGGING
	TComPtr<IDXGIDebug1> DXGIDebug;
	TComPtr<ID3D12Debug6> D3D12Debug;
#endif

	TComPtr<IDXGIFactory7> Factory;
};
