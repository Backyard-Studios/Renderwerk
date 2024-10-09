#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Graphics/GraphicsCommon.h"

enum class ENGINE_API EShaderStage : uint8
{
	None,
	Vertex,
	Pixel,
	RootSignature,
};

ENGINE_API std::string ToString(EShaderStage Stage);

enum class ENGINE_API EShaderConfiguration : uint8
{
	Debug,
	Release,
};

ENGINE_API std::string ToString(EShaderConfiguration Configuration);

struct ENGINE_API FShaderMacro
{
	std::string Name;
	std::string Value;
};

struct ENGINE_API FShaderCompilationDesc
{
	EShaderStage Stage;
	EShaderConfiguration Configuration = EShaderConfiguration::Release;
	TVector<FShaderMacro> Macros;
	bool bExtractReflection = false;
};

struct ENGINE_API FCompiledShader
{
	EShaderStage Stage = EShaderStage::None;
	ComPtr<IDxcBlob> ShaderBlob = nullptr;
	ComPtr<IDxcBlob> ReflectionBlob = nullptr;

	bool8 IsValid() const
	{
		return ShaderBlob != nullptr;
	}

	bool8 HasReflection() const
	{
		return ReflectionBlob != nullptr;
	}

	D3D12_SHADER_BYTECODE GetBytecode() const
	{
		D3D12_SHADER_BYTECODE Bytecode;
		Bytecode.pShaderBytecode = ShaderBlob->GetBufferPointer();
		Bytecode.BytecodeLength = ShaderBlob->GetBufferSize();
		return Bytecode;
	}
};

class ENGINE_API FShaderCompiler
{
public:
	FShaderCompiler();
	~FShaderCompiler();

	DELETE_COPY_AND_MOVE(FShaderCompiler);

public:
	FCompiledShader CompileFromFile(const std::string& FilePath, const FShaderCompilationDesc& Description) const;
	ComPtr<IDxcBlob> CompileRootSignature(const std::string& FilePath, const EShaderConfiguration& Configuration) const;

public:
	ComPtr<IDxcBlobEncoding> GetBufferFromFile(const std::string& FilePath) const;
	ComPtr<IDxcResult> Compile(const ComPtr<IDxcBlobEncoding>& Blob, TVector<LPCWSTR>& Arguments) const;

private:
	static TVector<LPCWSTR> GetArgumentsForDescription(const FShaderCompilationDesc& Description);

	static const wchar_t* GetEntrypointForStage(EShaderStage Stage);
	static const wchar_t* GetShaderModelTargetForStage(EShaderStage Stage);

private:
	ComPtr<IDxcLibrary> Library;
	ComPtr<IDxcCompiler3> Compiler;
	ComPtr<IDxcUtils> Utils;
};
