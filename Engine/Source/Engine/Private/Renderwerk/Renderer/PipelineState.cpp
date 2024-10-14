#include "pch.h"

#include "Renderwerk/Renderer/PipelineState.h"

FShader GetShaderOfType(const TVector<FShader>& Shaders, const EShaderType Type)
{
	for (const FShader& Shader : Shaders)
	{
		if (Shader.GetType() == Type)
			return Shader;
	}
	return FShader();
}

IPipelineState::IPipelineState(const EPipelineStateType InType)
	: Type(InType)
{
}

IPipelineState::~IPipelineState()
{
	PipelineState.Reset();
}

ComPtr<ID3D12PipelineState> IPipelineState::CreatePipelineState(const ComPtr<ID3D12Device14>& Device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& Desc)
{
	ComPtr<ID3D12PipelineState> PipelineState;
	CHECK_RESULT(Device->CreateGraphicsPipelineState(&Desc, IID_PPV_ARGS(PipelineState.GetAddressOf())), "Failed to create graphics pipeline state")
	return PipelineState;
}

ComPtr<ID3D12PipelineState> IPipelineState::CreatePipelineState(const ComPtr<ID3D12Device14>& Device, const D3D12_COMPUTE_PIPELINE_STATE_DESC& Desc)
{
	ComPtr<ID3D12PipelineState> PipelineState;
	CHECK_RESULT(Device->CreateComputePipelineState(&Desc, IID_PPV_ARGS(PipelineState.GetAddressOf())), "Failed to create graphics pipeline state")
	return PipelineState;
}

ComPtr<ID3D12PipelineState> IPipelineState::CreatePipelineState(const ComPtr<ID3D12Device14>& Device, const D3D12_PIPELINE_STATE_STREAM_DESC& Desc)
{
	ComPtr<ID3D12PipelineState> PipelineState;
	CHECK_RESULT(Device->CreatePipelineState(&Desc, IID_PPV_ARGS(PipelineState.GetAddressOf())), "Failed to create pipeline state")
	return PipelineState;
}

FGraphicsPipelineState::FGraphicsPipelineState(const ComPtr<ID3D12Device14>& Device, const FGraphicsPipelineStateDesc& InDesc)
	: IPipelineState(EPipelineStateType::Graphics)
{
	FShader VertexShader = GetShaderOfType(InDesc.Shaders, EShaderType::Vertex);
	RW_DEBUG_ASSERT(VertexShader.IsValid(), "Vertex shader is required for graphics pipeline state")

	FShader PixelShader = GetShaderOfType(InDesc.Shaders, EShaderType::Pixel);
	RW_DEBUG_ASSERT(PixelShader.IsValid(), "Pixel shader is required for graphics pipeline state")

	FShader GeometryShader = GetShaderOfType(InDesc.Shaders, EShaderType::Geometry);
	FShader DomainShader = GetShaderOfType(InDesc.Shaders, EShaderType::Domain);
	FShader HullShader = GetShaderOfType(InDesc.Shaders, EShaderType::Hull);

	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayoutDescs;
	for (const FInputElement& Element : InDesc.InputLayout.GetElements())
	{
		D3D12_INPUT_CLASSIFICATION InputSlotClass;
		if (Element.SlotType == FInputElementSlotType::PerInstanceData)
			InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
		else
			InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

		D3D12_INPUT_ELEMENT_DESC InputLayoutDesc = {};
		InputLayoutDesc.SemanticName = Element.SemanticName.c_str();
		InputLayoutDesc.SemanticIndex = Element.SemanticIndex;
		InputLayoutDesc.Format = Element.Format;
		InputLayoutDesc.InputSlot = Element.InputSlot;
		InputLayoutDesc.AlignedByteOffset = Element.AlignedByteOffset;
		InputLayoutDesc.InputSlotClass = InputSlotClass;
		InputLayoutDescs.push_back(InputLayoutDesc);
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc = {};
	PipelineStateDesc.pRootSignature = InDesc.RootSignature.Get();
	PipelineStateDesc.VS = VertexShader.GetBytecode();
	PipelineStateDesc.PS = PixelShader.GetBytecode();
	if (GeometryShader.IsValid())
		PipelineStateDesc.GS = GeometryShader.GetBytecode();
	if (DomainShader.IsValid())
		PipelineStateDesc.DS = DomainShader.GetBytecode();
	if (HullShader.IsValid())
		PipelineStateDesc.HS = HullShader.GetBytecode();
	PipelineStateDesc.RasterizerState = InDesc.RasterizerState;
	PipelineStateDesc.BlendState = InDesc.BlendState;
	PipelineStateDesc.DepthStencilState = InDesc.DepthStencilState;
	PipelineStateDesc.PrimitiveTopologyType = InDesc.PrimitiveTopologyType;
	PipelineStateDesc.NumRenderTargets = InDesc.NumRenderTargets;
	for (size64 Index = 0; Index < InDesc.RenderTargetFormats.size(); ++Index)
		PipelineStateDesc.RTVFormats[Index] = InDesc.RenderTargetFormats[Index];
	PipelineStateDesc.DSVFormat = InDesc.DepthStencilFormat;
	PipelineStateDesc.InputLayout.pInputElementDescs = InputLayoutDescs.data();
	PipelineStateDesc.InputLayout.NumElements = static_cast<uint32>(InputLayoutDescs.size());
	PipelineState = CreatePipelineState(Device, PipelineStateDesc);
}

FGraphicsPipelineState::~FGraphicsPipelineState()
{
}

FComputePipelineState::FComputePipelineState(const ComPtr<ID3D12Device14>& Device, const FComputePipelineStateDesc& InDesc)
	: IPipelineState(EPipelineStateType::Compute)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC PipelineStateDesc = {};
	PipelineStateDesc.pRootSignature = InDesc.RootSignature.Get();
	PipelineStateDesc.CS = InDesc.ComputeShader.GetBytecode();
	PipelineState = CreatePipelineState(Device, PipelineStateDesc);
}

FComputePipelineState::~FComputePipelineState()
{
}

FMeshShadingPipelineState::FMeshShadingPipelineState(const ComPtr<ID3D12Device14>& Device, const FMeshShadingPipelineStateDesc& InDesc)
	: IPipelineState(EPipelineStateType::MeshShading)
{
	FShader MeshShader = GetShaderOfType(InDesc.Shaders, EShaderType::Mesh);
	RW_DEBUG_ASSERT(MeshShader.IsValid(), "Mesh shader is required for mesh shading pipeline state")

	FShader PixelShader = GetShaderOfType(InDesc.Shaders, EShaderType::Pixel);
	RW_DEBUG_ASSERT(PixelShader.IsValid(), "Pixel shader is required for mesh shading pipeline state")

	FShader AmplificationShader = GetShaderOfType(InDesc.Shaders, EShaderType::Amplification);

	D3DX12_MESH_SHADER_PIPELINE_STATE_DESC PipelineStateDesc = {};
	PipelineStateDesc.pRootSignature = InDesc.RootSignature.Get();
	PipelineStateDesc.MS = MeshShader.GetBytecode();
	PipelineStateDesc.PS = PixelShader.GetBytecode();
	if (AmplificationShader.IsValid())
		PipelineStateDesc.AS = AmplificationShader.GetBytecode();
	PipelineStateDesc.RasterizerState = InDesc.RasterizerState;
	PipelineStateDesc.BlendState = InDesc.BlendState;
	PipelineStateDesc.DepthStencilState = InDesc.DepthStencilState;
	PipelineStateDesc.PrimitiveTopologyType = InDesc.PrimitiveTopologyType;
	PipelineStateDesc.NumRenderTargets = InDesc.NumRenderTargets;
	for (size64 Index = 0; Index < InDesc.RenderTargetFormats.size(); ++Index)
		PipelineStateDesc.RTVFormats[Index] = InDesc.RenderTargetFormats[Index];
	PipelineStateDesc.DSVFormat = InDesc.DepthStencilFormat;

	CD3DX12_PIPELINE_MESH_STATE_STREAM MeshStateStreamDesc(PipelineStateDesc);
	D3D12_PIPELINE_STATE_STREAM_DESC StreamDesc = {};
	StreamDesc.SizeInBytes = sizeof(MeshStateStreamDesc);
	StreamDesc.pPipelineStateSubobjectStream = &MeshStateStreamDesc;
	PipelineState = CreatePipelineState(Device, StreamDesc);
}

FMeshShadingPipelineState::~FMeshShadingPipelineState()
{
}
