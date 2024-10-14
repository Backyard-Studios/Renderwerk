#include "pch.h"

#include "Renderwerk/Renderer/InputLayout.h"

DXGI_FORMAT ConvertMaskToFormat(const uint32 Mask)
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

std::string ToString(const FInputElementSlotType Type)
{
	switch (Type)
	{
	case FInputElementSlotType::PerVertexData: return "PerVertexData";
	case FInputElementSlotType::PerInstanceData: return "PerInstanceData";
	default:
		return "None";
	}
}

FInputLayout::FInputLayout()
{
}

FInputLayout::FInputLayout(const TVector<FInputElement>& InElements)
	: Elements(InElements)
{
}

FInputLayout::FInputLayout(const FShader& Shader)
{
	ComPtr<IDxcUtils> DxcUtils;
	CHECK_RESULT(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(DxcUtils.GetAddressOf())), "Failed to create DXC utils")

	DxcBuffer Buffer = {};
	Buffer.Ptr = Shader.GetBytecode().pShaderBytecode;
	Buffer.Size = Shader.GetBytecode().BytecodeLength;

	ComPtr<ID3D12ShaderReflection> Reflection;
	CHECK_RESULT(DxcUtils->CreateReflection(&Buffer, IID_PPV_ARGS(Reflection.GetAddressOf())), "Failed to create shader reflection")

	D3D12_SHADER_DESC ShaderDesc = {};
	CHECK_RESULT(Reflection->GetDesc(&ShaderDesc), "Failed to get shader description")

	for (uint32 Index = 0; Index < ShaderDesc.InputParameters; ++Index)
	{
		D3D12_SIGNATURE_PARAMETER_DESC SignatureDesc = {};
		CHECK_RESULT(Reflection->GetInputParameterDesc(Index, &SignatureDesc), "Failed to get input parameter description")

		FInputElement Element = {};
		Element.SemanticName = SignatureDesc.SemanticName;
		Element.SemanticIndex = SignatureDesc.SemanticIndex;
		Element.Format = ConvertMaskToFormat(SignatureDesc.Mask);
		Element.InputSlot = SignatureDesc.Register;
		Element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		Element.SlotType = FInputElementSlotType::PerVertexData;
		Elements.push_back(Element);
	}
}

FInputLayout::~FInputLayout()
{
	Elements.clear();
}

void FInputLayout::AddElement(const FInputElement& Element)
{
	Elements.push_back(Element);
}
