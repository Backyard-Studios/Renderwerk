#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/Adapter.h"
#include "Renderwerk/Renderer/D3D12Include.h"
#include "Renderwerk/Renderer/Device.h"

struct ENGINE_API FRendererContextDesc
{
	bool8 bEnableDebugging = false;

	/**
	 * Requires bEnableDebugging to be true.
	 */
	bool8 bEnableValidation = false;
};

class ENGINE_API FRendererContext
{
public:
	FRendererContext(const FRendererContextDesc& InDesc);
	~FRendererContext();

	DELETE_COPY_AND_MOVE(FRendererContext);

public:
	[[nodiscard]] TVector<TSharedPtr<FAdapter>> GetAdapters() const;
	[[nodiscard]] TSharedPtr<FAdapter> GetPrimaryAdapter() const;
	[[nodiscard]] TSharedPtr<FAdapter> GetAdapter(const std::string& Name) const;
	[[nodiscard]] TSharedPtr<FAdapter> GetSuitableAdapter() const;

	[[nodiscard]] TSharedPtr<FDevice> CreateDevice(const TSharedPtr<FAdapter>& Adapter, const FDeviceDesc& Desc);

public:
	[[nodiscard]] ComPtr<IDXGIFactory7> GetFactory() const { return Factory; }

private:
	FRendererContextDesc Description;

	ComPtr<IDXGIDebug1> DXGIDebug;
	ComPtr<ID3D12Debug6> D3D12Debug;

	ComPtr<IDXGIFactory7> Factory;
};
