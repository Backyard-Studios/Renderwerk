#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/D3D12Include.h"
#include "Renderwerk/Renderer/InputLayout.h"
#include "Renderwerk/Renderer/ShaderCompiler.h"

enum class EPipelineStateType : uint8
{
	Graphics,
	Compute,
	MeshShading,
};

class ENGINE_API IPipelineState
{
public:
	IPipelineState(EPipelineStateType InType);
	virtual ~IPipelineState();

	DELETE_COPY_AND_MOVE(IPipelineState);

public:
	[[nodiscard]] ComPtr<ID3D12PipelineState> GetHandle() const { return PipelineState; }

	[[nodiscard]] EPipelineStateType GetType() const { return Type; }

protected:
	static ComPtr<ID3D12PipelineState> CreatePipelineState(const ComPtr<ID3D12Device14>& Device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& Desc);
	static ComPtr<ID3D12PipelineState> CreatePipelineState(const ComPtr<ID3D12Device14>& Device, const D3D12_COMPUTE_PIPELINE_STATE_DESC& Desc);
	static ComPtr<ID3D12PipelineState> CreatePipelineState(const ComPtr<ID3D12Device14>& Device, const D3D12_PIPELINE_STATE_STREAM_DESC& Desc);

protected:
	EPipelineStateType Type;
	ComPtr<ID3D12PipelineState> PipelineState;
};

struct ENGINE_API FGraphicsPipelineStateDesc
{
	ComPtr<ID3D12RootSignature> RootSignature;
	TVector<FShader> Shaders;
	CD3DX12_RASTERIZER_DESC2 RasterizerState = CD3DX12_RASTERIZER_DESC2(D3D12_DEFAULT);
	CD3DX12_BLEND_DESC BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	CD3DX12_DEPTH_STENCIL_DESC2 DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC2(D3D12_DEFAULT);
	D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	uint32 NumRenderTargets = 1;
	TArray<DXGI_FORMAT, 8> RenderTargetFormats = {DXGI_FORMAT_R8G8B8A8_UNORM};
	DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
	FInputLayout InputLayout = {};
};

class ENGINE_API FGraphicsPipelineState : public IPipelineState
{
public:
	FGraphicsPipelineState(const ComPtr<ID3D12Device14>& Device, const FGraphicsPipelineStateDesc& InDesc);
	~FGraphicsPipelineState() override;

	DELETE_COPY_AND_MOVE(FGraphicsPipelineState);

private:
};

struct ENGINE_API FComputePipelineStateDesc
{
	ComPtr<ID3D12RootSignature> RootSignature;
	FShader ComputeShader;
};

class ENGINE_API FComputePipelineState : public IPipelineState
{
public:
	FComputePipelineState(const ComPtr<ID3D12Device14>& Device, const FComputePipelineStateDesc& InDesc);
	~FComputePipelineState() override;

	DELETE_COPY_AND_MOVE(FComputePipelineState);

private:
};

struct ENGINE_API FMeshShadingPipelineStateDesc
{
	ComPtr<ID3D12RootSignature> RootSignature;
	TVector<FShader> Shaders;
	CD3DX12_RASTERIZER_DESC2 RasterizerState = CD3DX12_RASTERIZER_DESC2(D3D12_DEFAULT);
	CD3DX12_BLEND_DESC BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	CD3DX12_DEPTH_STENCIL_DESC2 DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC2(D3D12_DEFAULT);
	D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	uint32 NumRenderTargets = 1;
	TArray<DXGI_FORMAT, 8> RenderTargetFormats = {DXGI_FORMAT_R8G8B8A8_UNORM};
	DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
};

class ENGINE_API FMeshShadingPipelineState : public IPipelineState
{
public:
	FMeshShadingPipelineState(const ComPtr<ID3D12Device14>& Device, const FMeshShadingPipelineStateDesc& InDesc);
	~FMeshShadingPipelineState() override;

	DELETE_COPY_AND_MOVE(FMeshShadingPipelineState);

private:
};
