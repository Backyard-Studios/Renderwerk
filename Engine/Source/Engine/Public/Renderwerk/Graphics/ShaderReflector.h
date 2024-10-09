#pragma once

#include "ShaderCompiler.h"

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Graphics/GraphicsCommon.h"

class ENGINE_API FShaderReflector
{
public:
	FShaderReflector(const FCompiledShader& CompiledShader);
	~FShaderReflector();

	DELETE_COPY_AND_MOVE(FShaderReflector);

public:
	[[nodiscard]] TVector<D3D12_INPUT_ELEMENT_DESC> GetInputElements() const { return InputElements; }
	[[nodiscard]] TVector<D3D12_ROOT_PARAMETER1> GetRootParameters() const { return RootParameters; }

private:
	void ReflectInputElements(const D3D12_SHADER_DESC& ShaderDesc);
	void ReflectRootParameters(const D3D12_SHADER_DESC& ShaderDesc);

private:
	static DXGI_FORMAT ConvertMaskToFormat(uint32 Mask);

private:
	ComPtr<ID3D12ShaderReflection> Reflection;

	TVector<D3D12_INPUT_ELEMENT_DESC> InputElements;
	TVector<D3D12_ROOT_PARAMETER1> RootParameters;
};
