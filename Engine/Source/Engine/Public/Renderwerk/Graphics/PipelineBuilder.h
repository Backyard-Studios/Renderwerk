#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"

#include "Renderwerk/Graphics/GraphicsCommon.h"

/**
 * A builder class for creating pipeline states. It comes with an opinionated default configuration.
 * The available functions are growing as needed.
 */
class ENGINE_API FPipelineBuilder
{
public:
	FPipelineBuilder(const ComPtr<ID3D12RootSignature>& RootSignature);
	~FPipelineBuilder();

	DELETE_COPY_AND_MOVE(FPipelineBuilder)

public:
	FPipelineBuilder* SetVertexShader(const D3D12_SHADER_BYTECODE& Shader);
	FPipelineBuilder* SetPixelShader(const D3D12_SHADER_BYTECODE& Shader);
	FPipelineBuilder* SetDomainShader(const D3D12_SHADER_BYTECODE& Shader);
	FPipelineBuilder* SetHullShader(const D3D12_SHADER_BYTECODE& Shader);
	FPipelineBuilder* SetGeometryShader(const D3D12_SHADER_BYTECODE& Shader);

	FPipelineBuilder* SetFillMode(D3D12_FILL_MODE FillMode);
	FPipelineBuilder* SetCullMode(D3D12_CULL_MODE CullMode);

	FPipelineBuilder* DisableDepth();
	FPipelineBuilder* DisableStencil();

	FPipelineBuilder* AddInputElement(const D3D12_INPUT_ELEMENT_DESC& Element);

	FPipelineBuilder* SetRenderTargetFormat(const DXGI_FORMAT& Format);
	FPipelineBuilder* SetDepthStencilFormat(const DXGI_FORMAT& Format);

	FPipelineBuilder* SetSample(const DXGI_SAMPLE_DESC& SampleDesc);

	FPipelineBuilder* SetFlags(const D3D12_PIPELINE_STATE_FLAGS& Flags);
	FPipelineBuilder* AddFlags(const D3D12_PIPELINE_STATE_FLAGS& Flags);

	ComPtr<ID3D12PipelineState> Build(const TSharedPtr<FGraphicsDevice>& Device);

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC Description;

	TVector<D3D12_INPUT_ELEMENT_DESC> InputElements;
};
