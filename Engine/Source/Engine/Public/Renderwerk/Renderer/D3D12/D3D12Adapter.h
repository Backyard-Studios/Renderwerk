#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/Adapter.h"
#include "Renderwerk/Renderer/D3D12/D3D12Include.h"

class ENGINE_API FD3D12Adapter : public IAdapter
{
public:
	FD3D12Adapter(const ComPtr<IDXGIAdapter4>& InAdapter);
	~FD3D12Adapter() override;

	DELETE_COPY_AND_MOVE(FD3D12Adapter);

public:
	[[nodiscard]] std::string GetName() const override;

public:
	[[nodiscard]] ComPtr<IDXGIAdapter4> GetHandle() const { return Adapter; }

private:
	ComPtr<IDXGIAdapter4> Adapter;

	DXGI_ADAPTER_DESC3 Description;
	std::string AdapterName;
};
