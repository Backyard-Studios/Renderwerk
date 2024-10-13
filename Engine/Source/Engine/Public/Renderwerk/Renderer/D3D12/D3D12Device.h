#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/Device.h"
#include "Renderwerk/Renderer/D3D12/D3D12Include.h"

class ENGINE_API FD3D12Device : public IDevice
{
public:
	FD3D12Device(const TSharedPtr<IAdapter>& InAdapter);
	~FD3D12Device() override;

	DELETE_COPY_AND_MOVE(FD3D12Device);

public:
	[[nodiscard]] ComPtr<ID3D12Device14> GetHandle() const { return Device; }

private:
	ComPtr<ID3D12Device14> Device;
};
