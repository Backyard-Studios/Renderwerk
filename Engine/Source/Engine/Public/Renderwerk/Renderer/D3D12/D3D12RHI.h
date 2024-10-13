#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/Device.h"
#include "Renderwerk/Renderer/RHI.h"
#include "Renderwerk/Renderer/D3D12/D3D12Include.h"

class ENGINE_API FD3D12RHI : public IRHI
{
public:
	FD3D12RHI(const FRHIDesc& InDesc);
	~FD3D12RHI() override;

	DELETE_COPY_AND_MOVE(FD3D12RHI);

public:
	[[nodiscard]] TVector<TSharedPtr<IAdapter>> GetAdapters() override;
	[[nodiscard]] TSharedPtr<IAdapter> GetPrimaryAdapter() override;
	[[nodiscard]] TSharedPtr<IAdapter> GetAdapter(const std::string& Name) override;
	[[nodiscard]] TSharedPtr<IAdapter> GetSuitableAdapter() override;

	[[nodiscard]] TSharedPtr<IDevice> CreateDevice(const TSharedPtr<IAdapter>& Adapter, const FDeviceDesc& Desc) override;

private:
	ComPtr<IDXGIDebug1> DXGIDebug;
	ComPtr<ID3D12Debug6> D3D12Debug;

	ComPtr<IDXGIFactory7> Factory;
};
