#include "pch.h"

#include "Renderwerk/Renderer/ShaderCompiler.h"

std::string ToString(const EShaderType Type)
{
	switch (Type)
	{
	case EShaderType::Vertex: return "Vertex";
	case EShaderType::Pixel: return "Pixel";
	case EShaderType::Geometry: return "Geometry";
	case EShaderType::Domain: return "Domain";
	case EShaderType::Hull: return "Hull";
	case EShaderType::Compute: return "Compute";
	case EShaderType::Amplification: return "Amplification";
	case EShaderType::Mesh: return "Mesh";
	case EShaderType::RootSignature: return "RootSignature";
	case EShaderType::None:
	default:
		return "None";
	}
}

FShader::FShader()
{
}

FShader::FShader(const EShaderType InType, const ComPtr<IDxcBlob>& InBlob, const ComPtr<IDxcBlob>& InReflectionBlob)
	: Type(InType), Blob(InBlob), ReflectionBlob(InReflectionBlob)
{
}

FShader::~FShader()
{
}

bool8 FShader::IsValid() const
{
	return Type != EShaderType::None && Blob != nullptr;
}

bool8 FShader::HasReflectionData() const
{
	return ReflectionBlob != nullptr;
}

D3D12_SHADER_BYTECODE FShader::GetBytecode() const
{
	return {Blob->GetBufferPointer(), Blob->GetBufferSize()};
}

FShaderCompiler::FShaderCompiler(const FShaderCompilerDesc& InDescription)
	: Description(InDescription)
{
	CHECK_RESULT(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&DxcLibrary)), "Failed to create DXC library")
	CHECK_RESULT(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&DxcUtils)), "Failed to create DXC utils")
	CHECK_RESULT(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&DxcCompiler)), "Failed to create DXC compiler")

	CHECK_RESULT(DxcLibrary->CreateIncludeHandler(&IncludeHandler), "Failed to create DXC include handler")

	ComPtr<IDxcVersionInfo3> VersionInfo;
	CHECK_RESULT(DxcCompiler.As(&VersionInfo), "Failed to get DXC version info")
	char* VersionString = nullptr;
	CHECK_RESULT(VersionInfo->GetCustomVersionString(&VersionString), "Failed to get DXC version string")
	RW_LOG_INFO("DXC Version: {}", VersionString);
	CoTaskMemFree(VersionString);
}

FShaderCompiler::~FShaderCompiler()
{
	DxcCompiler.Reset();
	DxcUtils.Reset();
	DxcLibrary.Reset();
}

FShader FShaderCompiler::Compile(const FCompileDesc& CompileDesc) const
{
	DxcBuffer SourceBuffer = {};
	SourceBuffer.Ptr = CompileDesc.Source->GetBufferPointer();
	SourceBuffer.Size = CompileDesc.Source->GetBufferSize();

	TVector<LPCWSTR> Arguments = GetArguments(CompileDesc, Description.ShaderModel);
	uint32 ArgumentsSize = static_cast<uint32>(Arguments.size());

	ComPtr<IDxcResult> CompilationResult;
	HRESULT CompileResult = DxcCompiler->Compile(&SourceBuffer, Arguments.data(), ArgumentsSize, IncludeHandler.Get(), IID_PPV_ARGS(&CompilationResult));
	CHECK_RESULT(CompileResult, "Failed to compile shader")

	ComPtr<IDxcBlobUtf8> ErrorBlob;
	CHECK_RESULT(CompilationResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&ErrorBlob), nullptr), "Failed to get error blob")
	if (ErrorBlob && ErrorBlob->GetStringLength() > 0)
	{
		std::string ErrorString = std::string(ErrorBlob->GetStringPointer(), ErrorBlob->GetStringLength());
		RW_LOG_ERROR("Shader compilation failed with errors:");
		RW_LOG_ERROR("{}", ErrorString);
		RW_ASSERT_CRITICAL(false, "Shader compilation failed")
	}

	HRESULT Status;
	CHECK_RESULT(CompilationResult->GetStatus(&Status), "Failed to get shader compilation status")
	CHECK_RESULT(Status, "Failed to get shader compilation status")

	ComPtr<IDxcBlob> ShaderBlob;
	CHECK_RESULT(CompilationResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&ShaderBlob), nullptr), "Failed to get shader blob")

	ComPtr<IDxcBlob> ReflectionBlob = nullptr;
	if (CompileDesc.bExtractReflectionData)
		CHECK_RESULT(CompilationResult->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&ReflectionBlob), nullptr), "Failed to get reflection blob")

	return FShader(CompileDesc.Type, ShaderBlob, ReflectionBlob);
}

ComPtr<IDxcBlobEncoding> FShaderCompiler::GetBlobFromFile(const std::string& FilePath) const
{
	uint32 CodePage = DXC_CP_UTF8;
	ComPtr<IDxcBlobEncoding> SourceBlob;
	CHECK_RESULT(DxcLibrary->CreateBlobFromFile(ToWide(FilePath.c_str()).c_str(), &CodePage, &SourceBlob), "Failed to create source blob from file")
	return SourceBlob;
}

TVector<LPCWSTR> FShaderCompiler::GetArguments(const FCompileDesc& CompileDesc, const D3D_SHADER_MODEL ShaderModel)
{
	TVector<LPCWSTR> Arguments;
	Arguments.push_back(L"-Qstrip_debug");
	Arguments.push_back(L"-Qstrip_reflect");
	if (CompileDesc.bTreatWarningsAsErrors)
		Arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);
	Arguments.push_back(GetArgumentFromConfig(CompileDesc.Config));
	Arguments.push_back(L"-I");
	std::wstring IncludeDirectory = ToWide(FDirectories::GetShadersDirectory().c_str());
	wchar_t* NewIncludeDirectory = new wchar_t[IncludeDirectory.length() + 1];
	wcscpy_s(NewIncludeDirectory, IncludeDirectory.length() + 1, IncludeDirectory.c_str());
	Arguments.push_back(NewIncludeDirectory);
	Arguments.push_back(L"-E");
	Arguments.push_back(GetEntrypointForType(CompileDesc.Type));
	Arguments.push_back(L"-T");
	std::wstring ShaderModelTarget = GetShaderModelTargetForType(CompileDesc.Type, ShaderModel);
	wchar_t* NewShaderModelTarget = new wchar_t[ShaderModelTarget.length() + 1];
	wcscpy_s(NewShaderModelTarget, ShaderModelTarget.length() + 1, ShaderModelTarget.c_str());
	Arguments.push_back(NewShaderModelTarget);
	return Arguments;
}

LPCWSTR FShaderCompiler::GetArgumentFromConfig(const EShaderCompileConfig Config)
{
	switch (Config)
	{
	case EShaderCompileConfig::Debug: return DXC_ARG_DEBUG;
	case EShaderCompileConfig::Optimization: return DXC_ARG_OPTIMIZATION_LEVEL1;
	case EShaderCompileConfig::FullOptimization: return DXC_ARG_OPTIMIZATION_LEVEL3;
	default:
		return DXC_ARG_SKIP_OPTIMIZATIONS;
	}
}

LPCWSTR FShaderCompiler::GetEntrypointForType(const EShaderType Type)
{
	switch (Type)
	{
	case EShaderType::Vertex: return L"VSMain";
	case EShaderType::Pixel: return L"PSMain";
	case EShaderType::Geometry: return L"GSMain";
	case EShaderType::Domain: return L"DSMain";
	case EShaderType::Hull: return L"HSMain";
	case EShaderType::Compute: return L"CSMain";
	case EShaderType::Amplification: return L"ASMain";
	case EShaderType::Mesh: return L"MSMain";
	case EShaderType::RootSignature: return L"ROOT_SIGNATURE";
	case EShaderType::None:
	default:
		return L"";
	}
}

std::wstring FShaderCompiler::GetShaderModelTargetForType(const EShaderType Type, const D3D_SHADER_MODEL ShaderModel)
{
	switch (Type)
	{
	case EShaderType::Vertex: return std::format(L"vs_{}", GetShaderModelString(ShaderModel));
	case EShaderType::Pixel: return std::format(L"ps_{}", GetShaderModelString(ShaderModel));
	case EShaderType::Geometry: return std::format(L"gs_{}", GetShaderModelString(ShaderModel));
	case EShaderType::Domain: return std::format(L"ds_{}", GetShaderModelString(ShaderModel));
	case EShaderType::Hull: return std::format(L"hs_{}", GetShaderModelString(ShaderModel));
	case EShaderType::Compute: return std::format(L"cs_{}", GetShaderModelString(ShaderModel));
	case EShaderType::Amplification: return std::format(L"as_{}", GetShaderModelString(ShaderModel));
	case EShaderType::Mesh: return std::format(L"ms_{}", GetShaderModelString(ShaderModel));
	case EShaderType::RootSignature: return L"rootsig_1_1";
	case EShaderType::None:
	default:
		return L"None";
	}
}

LPCWSTR FShaderCompiler::GetShaderModelString(const D3D_SHADER_MODEL ShaderModel)
{
	switch (ShaderModel)
	{
	case D3D_SHADER_MODEL_5_1: return L"5_1";
	case D3D_SHADER_MODEL_6_0: return L"6_0";
	case D3D_SHADER_MODEL_6_1: return L"6_1";
	case D3D_SHADER_MODEL_6_2: return L"6_2";
	case D3D_SHADER_MODEL_6_3: return L"6_3";
	case D3D_SHADER_MODEL_6_4: return L"6_4";
	case D3D_SHADER_MODEL_6_5: return L"6_5";
	case D3D_SHADER_MODEL_6_6: return L"6_6";
	case D3D_SHADER_MODEL_6_7: return L"6_7";
	case D3D_SHADER_MODEL_6_8: return L"6_8";
	case D3D_SHADER_MODEL_6_9: return L"6_9";
	case D3D_SHADER_MODEL_NONE:
	default:
		return L"None";
	}
}
