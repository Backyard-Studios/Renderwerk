#include "pch.h"

#include "Renderwerk/Graphics/GraphicsPipeline.h"

#include "Renderwerk/Graphics/PipelineBuilder.h"
#include "Renderwerk/Graphics/ShaderReflector.h"

FGraphicsPipeline::FGraphicsPipeline(const TSharedPtr<FGraphicsDevice>& Device, const FGraphicsPipelineDesc& InDescription)
	: Description(InDescription)
{
	HRESULT RootSignatureCreateResult = Device->GetHandle()->CreateRootSignature(0, Description.RootSignatureBlob->GetBufferPointer(),
	                                                                             Description.RootSignatureBlob->GetBufferSize(),
	                                                                             IID_PPV_ARGS(RootSignature.GetAddressOf()));
	CHECK_RESULT(RootSignatureCreateResult, "Failed to create root signature")

	TUniquePtr<FPipelineBuilder> Builder = MakeUnique<FPipelineBuilder>(RootSignature);
	Builder->SetFillMode(Description.FillMode);
	Builder->SetCullMode(Description.CullMode);
	Builder->SetRenderTargetCount(Description.RenderTargetCount);
	for (uint32 Index = 0; Index < Description.RenderTargetCount; ++Index)
		Builder->SetRenderTargetFormat(Index, Description.RenderTargetFormats.at(Index));
	if (!Description.bEnableDepth)
		Builder->DisableDepth();
	Builder->SetDepthStencilFormat(Description.DepthFormat);

	for (const FCompiledShader& Shader : Description.Shaders)
	{
		switch (Shader.Stage)
		{
		case EShaderStage::Vertex:
			{
				Builder->SetVertexShader(Shader.GetBytecode());

				TUniquePtr<FShaderReflector> Reflector = MakeUnique<FShaderReflector>(Shader);
				for (const D3D12_INPUT_ELEMENT_DESC& Element : Reflector->GetInputElements())
					Builder->AddInputElement(Element);
			}
			break;
		case EShaderStage::Pixel:
			{
				Builder->SetPixelShader(Shader.GetBytecode());
			}
			break;
		case EShaderStage::RootSignature:
		case EShaderStage::None:
		default:
			break;
		}
	}

	PipelineState = Builder->Build(Device);
}

FGraphicsPipeline::~FGraphicsPipeline()
{
	RootSignature.Reset();
	PipelineState.Reset();
}
