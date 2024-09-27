#include "pch.h"

#include "Renderwerk/Graphics/ShaderReflector.h"

FShaderReflector::FShaderReflector(const FCompiledShader& CompiledShader)
{
	RW_DEBUG_ASSERT(CompiledShader.HasReflection(), "Compiled shader must have reflection data")

	ComPtr<IDxcUtils> DxcUtils;
	CHECK_RESULT(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&DxcUtils)), "Failed to create DXC utils")

	DxcBuffer Buffer;
	Buffer.Ptr = CompiledShader.ReflectionBlob->GetBufferPointer();
	Buffer.Size = CompiledShader.ReflectionBlob->GetBufferSize();
	Buffer.Encoding = 0;

	CHECK_RESULT(DxcUtils->CreateReflection(&Buffer, IID_PPV_ARGS(&Reflection)), "Failed to create shader reflection")

	D3D12_SHADER_DESC ShaderDesc = {};
	CHECK_RESULT(Reflection->GetDesc(&ShaderDesc), "Failed to get shader description")

	if (CompiledShader.Stage == EShaderStage::Vertex)
		ReflectInputElements(ShaderDesc);
	ReflectRootParameters(ShaderDesc);

	DxcUtils.Reset();
}

FShaderReflector::~FShaderReflector()
{
	RootParameters.clear();
	InputElements.clear();
	Reflection.Reset();
}

void FShaderReflector::ReflectInputElements(const D3D12_SHADER_DESC& ShaderDesc)
{
	InputElements.resize(ShaderDesc.InputParameters);
	for (uint32 Index = 0; Index < ShaderDesc.InputParameters; ++Index)
	{
		D3D12_SIGNATURE_PARAMETER_DESC ParameterDesc = {};
		CHECK_RESULT(Reflection->GetInputParameterDesc(Index, &ParameterDesc), "Failed to get input parameter description")

		D3D12_INPUT_ELEMENT_DESC ElementDesc;
		ElementDesc.SemanticName = ParameterDesc.SemanticName;
		ElementDesc.SemanticIndex = ParameterDesc.SemanticIndex;
		ElementDesc.Format = ConvertMaskToFormat(ParameterDesc.Mask);
		ElementDesc.InputSlot = 0;
		ElementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		ElementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		ElementDesc.InstanceDataStepRate = 0;
		InputElements[Index] = ElementDesc;
	}
}

void FShaderReflector::ReflectRootParameters(const D3D12_SHADER_DESC& ShaderDesc)
{
	RootParameters.resize(ShaderDesc.BoundResources);

	TVector<D3D12_DESCRIPTOR_RANGE1> DescriptorRanges;
	for (uint32 Index = 0; Index < ShaderDesc.BoundResources; ++Index)
	{
		D3D12_SHADER_INPUT_BIND_DESC InputBindDesc = {};
		CHECK_RESULT(Reflection->GetResourceBindingDesc(Index, &InputBindDesc), "Failed to get resource binding description")

		if (InputBindDesc.Type == D3D_SIT_CBUFFER)
		{
			ID3D12ShaderReflectionConstantBuffer* ConstantBuffer = Reflection->GetConstantBufferByName(InputBindDesc.Name);
			D3D12_SHADER_BUFFER_DESC BufferDesc = {};
			CHECK_RESULT(ConstantBuffer->GetDesc(&BufferDesc), "Failed to get constant buffer description")

			D3D12_ROOT_PARAMETER1 RootParameter = {};
			RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			RootParameter.Descriptor = {};
			RootParameter.Descriptor.ShaderRegister = InputBindDesc.BindPoint;
			RootParameter.Descriptor.RegisterSpace = InputBindDesc.Space;
			RootParameter.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
			RootParameter.DescriptorTable = {};
			RootParameter.Constants = {};
			RootParameters[Index] = RootParameter;
		}
		else if (InputBindDesc.Type == D3D_SIT_TEXTURE)
		{
			D3D12_DESCRIPTOR_RANGE1 DescriptorRange = {};
			DescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			DescriptorRange.NumDescriptors = 1;
			DescriptorRange.BaseShaderRegister = InputBindDesc.BindPoint;
			DescriptorRange.RegisterSpace = InputBindDesc.Space;
			DescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			DescriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
			DescriptorRanges.push_back(DescriptorRange);
		}
	}

	// This is a temporary solution, we need to handle multiple descriptor ranges
	if (!DescriptorRanges.empty())
	{
		D3D12_ROOT_PARAMETER1 RootParameter = {};
		RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		RootParameter.Descriptor = {};
		RootParameter.DescriptorTable = {};
		RootParameter.DescriptorTable.NumDescriptorRanges = DescriptorRanges.size();
		RootParameter.DescriptorTable.pDescriptorRanges = DescriptorRanges.data();
		RootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		RootParameters.push_back(RootParameter);
	}
}

DXGI_FORMAT FShaderReflector::ConvertMaskToFormat(const uint32 Mask)
{
	switch (Mask)
	{
	case 1: return DXGI_FORMAT_R32_FLOAT;
	case 3: return DXGI_FORMAT_R32G32_FLOAT;
	case 7: return DXGI_FORMAT_R32G32B32_FLOAT;
	case 15: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	default: return DXGI_FORMAT_UNKNOWN;
	}
}
