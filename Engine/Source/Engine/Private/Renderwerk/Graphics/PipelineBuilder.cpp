#include "pch.h"

#include "Renderwerk/Graphics/PipelineBuilder.h"

#include "Renderwerk/Graphics/GraphicsDevice.h"

FPipelineBuilder::FPipelineBuilder(const ComPtr<ID3D12RootSignature>& RootSignature)
	: Description({})
{
	Description.pRootSignature = RootSignature.Get();

	Description.VS = {};
	Description.PS = {};
	Description.DS = {};
	Description.HS = {};
	Description.GS = {};

	Description.StreamOutput = {};
	Description.StreamOutput.pSODeclaration = nullptr;
	Description.StreamOutput.NumEntries = 0;
	Description.StreamOutput.pBufferStrides = nullptr;
	Description.StreamOutput.NumStrides = 0;
	Description.StreamOutput.RasterizedStream = 0;

	Description.BlendState = {};
	Description.BlendState.AlphaToCoverageEnable = false;
	Description.BlendState.IndependentBlendEnable = false;
	for (int Index = 0; Index < _countof(Description.BlendState.RenderTarget); ++Index)
	{
		Description.BlendState.RenderTarget[Index] = {};
		Description.BlendState.RenderTarget[Index].BlendEnable = false;
		Description.BlendState.RenderTarget[Index].LogicOpEnable = false;
		Description.BlendState.RenderTarget[Index].SrcBlend = D3D12_BLEND_ONE;
		Description.BlendState.RenderTarget[Index].DestBlend = D3D12_BLEND_ZERO;
		Description.BlendState.RenderTarget[Index].BlendOp = D3D12_BLEND_OP_ADD;
		Description.BlendState.RenderTarget[Index].SrcBlendAlpha = D3D12_BLEND_ONE;
		Description.BlendState.RenderTarget[Index].DestBlendAlpha = D3D12_BLEND_ZERO;
		Description.BlendState.RenderTarget[Index].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		Description.BlendState.RenderTarget[Index].LogicOp = D3D12_LOGIC_OP_NOOP;
		Description.BlendState.RenderTarget[Index].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}

	Description.SampleMask = 0xFFFFFFFF;

	Description.RasterizerState = {};
	Description.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	Description.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	Description.RasterizerState.FrontCounterClockwise = false;
	Description.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	Description.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	Description.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	Description.RasterizerState.DepthClipEnable = true;
	Description.RasterizerState.MultisampleEnable = false;
	Description.RasterizerState.AntialiasedLineEnable = false;
	Description.RasterizerState.ForcedSampleCount = 0;
	Description.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	Description.DepthStencilState = {};
	Description.DepthStencilState.DepthEnable = false;
	Description.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	Description.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	Description.DepthStencilState.StencilEnable = false;
	Description.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	Description.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	Description.DepthStencilState.FrontFace = {};
	Description.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	Description.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	Description.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	Description.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	Description.DepthStencilState.BackFace = {};
	Description.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	Description.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	Description.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	Description.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	Description.InputLayout = {};
	Description.InputLayout.pInputElementDescs = nullptr;
	Description.InputLayout.NumElements = 0;

	Description.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	Description.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	Description.NumRenderTargets = 1;
	for (int Index = 0; Index < Description.NumRenderTargets; ++Index)
		Description.RTVFormats[Index] = DXGI_FORMAT_R8G8B8A8_UNORM;
	Description.DSVFormat = DXGI_FORMAT_UNKNOWN;

	Description.SampleDesc = {};
	Description.SampleDesc.Count = 1;
	Description.SampleDesc.Quality = 0;

	Description.NodeMask = 0;

	Description.CachedPSO = {};
	Description.CachedPSO.pCachedBlob = nullptr;
	Description.CachedPSO.CachedBlobSizeInBytes = 0;

	Description.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
}

FPipelineBuilder::~FPipelineBuilder()
{
}

FPipelineBuilder* FPipelineBuilder::SetVertexShader(const D3D12_SHADER_BYTECODE& Shader)
{
	Description.VS = Shader;
	return this;
}

FPipelineBuilder* FPipelineBuilder::SetPixelShader(const D3D12_SHADER_BYTECODE& Shader)
{
	Description.PS = Shader;
	return this;
}

FPipelineBuilder* FPipelineBuilder::SetDomainShader(const D3D12_SHADER_BYTECODE& Shader)
{
	Description.DS = Shader;
	return this;
}

FPipelineBuilder* FPipelineBuilder::SetHullShader(const D3D12_SHADER_BYTECODE& Shader)
{
	Description.HS = Shader;
	return this;
}

FPipelineBuilder* FPipelineBuilder::SetGeometryShader(const D3D12_SHADER_BYTECODE& Shader)
{
	Description.GS = Shader;
	return this;
}

FPipelineBuilder* FPipelineBuilder::SetFillMode(const D3D12_FILL_MODE FillMode)
{
	Description.RasterizerState.FillMode = FillMode;
	return this;
}

FPipelineBuilder* FPipelineBuilder::SetCullMode(const D3D12_CULL_MODE CullMode)
{
	Description.RasterizerState.CullMode = CullMode;
	return this;
}

FPipelineBuilder* FPipelineBuilder::DisableDepth()
{
	Description.DepthStencilState.DepthEnable = false;
	return this;
}

FPipelineBuilder* FPipelineBuilder::DisableStencil()
{
	Description.DepthStencilState.StencilEnable = false;
	return this;
}

FPipelineBuilder* FPipelineBuilder::AddInputElement(const D3D12_INPUT_ELEMENT_DESC& Element)
{
	InputElements.push_back(Element);
	return this;
}

FPipelineBuilder* FPipelineBuilder::SetRenderTargetFormat(const DXGI_FORMAT& Format)
{
	for (int Index = 0; Index < Description.NumRenderTargets; ++Index)
		Description.RTVFormats[Index] = Format;
	return this;
}

FPipelineBuilder* FPipelineBuilder::SetDepthStencilFormat(const DXGI_FORMAT& Format)
{
	Description.DSVFormat = Format;
	return this;
}

FPipelineBuilder* FPipelineBuilder::SetSample(const DXGI_SAMPLE_DESC& SampleDesc)
{
	Description.SampleDesc = SampleDesc;
	return this;
}

FPipelineBuilder* FPipelineBuilder::SetFlags(const D3D12_PIPELINE_STATE_FLAGS& Flags)
{
	Description.Flags = Flags;
	return this;
}

FPipelineBuilder* FPipelineBuilder::AddFlags(const D3D12_PIPELINE_STATE_FLAGS& Flags)
{
	Description.Flags |= Flags;
	return this;
}

ComPtr<ID3D12PipelineState> FPipelineBuilder::Build(const TSharedPtr<FGraphicsDevice>& Device)
{
	Description.InputLayout.pInputElementDescs = InputElements.data();
	Description.InputLayout.NumElements = static_cast<uint32>(InputElements.size());
	return Device->CreatePipelineState(Description);
}
