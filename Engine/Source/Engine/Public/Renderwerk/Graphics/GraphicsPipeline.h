#pragma once

#include "GraphicsDevice.h"
#include "ShaderCompiler.h"

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Graphics/D3D12Include.h"

struct ENGINE_API FGraphicsPipelineDesc
{
	ComPtr<IDxcBlob> RootSignatureBlob;
	TVector<FCompiledShader> Shaders;
	D3D12_FILL_MODE FillMode = D3D12_FILL_MODE_SOLID;
	D3D12_CULL_MODE CullMode = D3D12_CULL_MODE_BACK;
	D3D12_COMPARISON_FUNC DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	uint32 RenderTargetCount = 1;
	TArray<DXGI_FORMAT, 8> RenderTargetFormats = {
		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,
	};
	bool8 bEnableDepth = true;
	DXGI_FORMAT DepthFormat = DXGI_FORMAT_D32_FLOAT;
};

class ENGINE_API FGraphicsPipeline
{
public:
	FGraphicsPipeline(const TSharedPtr<FGraphicsDevice>& Device, const FGraphicsPipelineDesc& InDescription);
	~FGraphicsPipeline();

	DELETE_COPY_AND_MOVE(FGraphicsPipeline);

public:
	[[nodiscard]] ComPtr<ID3D12PipelineState> GetHandle() const { return PipelineState; }

private:
	FGraphicsPipelineDesc Description;

	ComPtr<ID3D12RootSignature> RootSignature;
	ComPtr<ID3D12PipelineState> PipelineState;
};
