#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/D3D12Include.h"

enum class ENGINE_API EShaderType : uint8
{
	None,
	Vertex,
	Pixel,
	Compute,
	Amplification,
	Mesh,
	RootSignature,
};

ENGINE_API std::string ToString(EShaderType Type);

struct ENGINE_API FShader
{
public:
	FShader();
	FShader(EShaderType InType, const ComPtr<IDxcBlob>& InBlob, const ComPtr<IDxcBlob>& InReflectionBlob);
	~FShader();

	DEFINE_DEFAULT_COPY_AND_MOVE(FShader);

public:
	[[nodiscard]] bool8 IsValid() const;
	[[nodiscard]] bool8 HasReflectionData() const;
	[[nodiscard]] D3D12_SHADER_BYTECODE GetBytecode() const;

public:
	[[nodiscard]] EShaderType GetType() const { return Type; }

	[[nodiscard]] ComPtr<IDxcBlob> GetData() const { return Blob; }
	[[nodiscard]] ComPtr<IDxcBlob> GetReflectionData() const { return ReflectionBlob; }

private:
	EShaderType Type = EShaderType::None;

	ComPtr<IDxcBlob> Blob;
	ComPtr<IDxcBlob> ReflectionBlob;
};

enum class ENGINE_API EShaderCompileConfig : uint8
{
	Debug,
	Optimization,
	FullOptimization
};

struct ENGINE_API FCompileDesc
{
	EShaderType Type = EShaderType::None;
	ComPtr<IDxcBlobEncoding> Source;
	EShaderCompileConfig Config = EShaderCompileConfig::FullOptimization;
	bool8 bTreatWarningsAsErrors = false;
	bool8 bExtractReflectionData = false;
};

struct ENGINE_API FShaderCompilerDesc
{
	D3D_SHADER_MODEL ShaderModel = D3D_SHADER_MODEL_6_9;
};

class ENGINE_API FShaderCompiler
{
public:
	FShaderCompiler(const FShaderCompilerDesc& InDescription);
	~FShaderCompiler();

	DELETE_COPY_AND_MOVE(FShaderCompiler);

public:
	[[nodiscard]] FShader Compile(const FCompileDesc& CompileDesc) const;

	[[nodiscard]] ComPtr<IDxcBlobEncoding> GetBlobFromFile(const std::string& FilePath) const;

private:
	static TVector<LPCWSTR> GetArguments(const FCompileDesc& CompileDesc, D3D_SHADER_MODEL ShaderModel);
	static LPCWSTR GetArgumentFromConfig(EShaderCompileConfig Config);

	static LPCWSTR GetEntrypointForType(EShaderType Type);
	static std::wstring GetShaderModelTargetForType(EShaderType Type, D3D_SHADER_MODEL ShaderModel);

	static LPCWSTR GetShaderModelString(D3D_SHADER_MODEL ShaderModel);

private:
	FShaderCompilerDesc Description;

	ComPtr<IDxcLibrary> DxcLibrary;
	ComPtr<IDxcUtils> DxcUtils;
	ComPtr<IDxcCompiler3> DxcCompiler;

	ComPtr<IDxcIncludeHandler> IncludeHandler;
};
