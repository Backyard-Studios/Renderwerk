#pragma once


#include "Swapchain.h"

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Memory/SmartPointers.h"

#include "Renderwerk/Graphics/D3D12Include.h"
#include "Renderwerk/Graphics/GraphicsAdapter.h"

class ENGINE_API FGraphicsContext
{
public:
	FGraphicsContext();
	~FGraphicsContext();

	DELETE_COPY_AND_MOVE(FGraphicsContext)

public:
	uint32 GetAdapterCount() const;
	TVector<TSharedPtr<FGraphicsAdapter>> GetAdapters() const;

	[[nodiscard]] TSharedPtr<FSwapchain> CreateSwapchain(const FSwapchainDesc& Description);

public:
	static TSharedPtr<FGraphicsAdapter> GetSuitableAdapter(const TVector<TSharedPtr<FGraphicsAdapter>>& AvailableAdapters,
	                                                       const FGraphicsAdapterRequirements& Requirements);

public:
	[[nodiscard]] ComPtr<IDXGIFactory6> GetFactory() const { return Factory; }

private:
#if RW_ENABLE_D3D12_DEBUG_LAYER
	ComPtr<IDXGIDebug1> DXGIDebug;
	ComPtr<ID3D12Debug6> D3D12Debug;
#endif

	ComPtr<IDXGIFactory6> Factory;
};
