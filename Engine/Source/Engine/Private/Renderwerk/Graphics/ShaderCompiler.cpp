#include "pch.h"

#include "Renderwerk/Graphics/ShaderCompiler.h"

std::string ToString(const EShaderStage Stage)
{
	switch (Stage)
	{
	DEFINE_ENUM_STRING_CASE(EShaderStage, Vertex);
	DEFINE_ENUM_STRING_CASE(EShaderStage, Pixel);
	DEFINE_ENUM_STRING_CASE(EShaderStage, RootSignature);
	case EShaderStage::None:
	default:
		return "Unknown";
	}
}

std::string ToString(const EShaderConfiguration Configuration)
{
	switch (Configuration)
	{
	DEFINE_ENUM_STRING_CASE(EShaderConfiguration, Debug);
	DEFINE_ENUM_STRING_CASE(EShaderConfiguration, Release);
	default:
		return "Unknown";
	}
}

FShaderCompiler::FShaderCompiler()
{
	CHECK_RESULT(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&Library)), "Failed to create DXC library")
	CHECK_RESULT(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&Compiler)), "Failed to create DXC compiler")
	CHECK_RESULT(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&Utils)), "Failed to create DXC utils")

	ComPtr<IDxcVersionInfo3> VersionInfo;
	CHECK_RESULT(Compiler.As(&VersionInfo), "Failed to get DXC version info")
	char* VersionString = nullptr;
	CHECK_RESULT(VersionInfo->GetCustomVersionString(&VersionString), "Failed to get DXC version string")
	RW_LOG_INFO("DXC Version: {}", VersionString);
	CoTaskMemFree(VersionString);
}

FShaderCompiler::~FShaderCompiler()
{
}

FCompiledShader FShaderCompiler::CompileFromFile(const std::string& FilePath, const FShaderCompilationDesc& Description) const
{
	RW_LOG_INFO("Compiling {} shader \"{}\" with {} configuration", ToString(Description.Stage), FilePath, ToString(Description.Configuration));

	TVector<LPCWSTR> Arguments = GetArgumentsForDescription(Description);

	Arguments.push_back(L"-I");
	Arguments.push_back(ToWide(FDirectories::GetShadersDirectory().c_str()).c_str());

	ComPtr<IDxcBlobEncoding> SourceBlob = GetBufferFromFile(FilePath);

	ComPtr<IDxcResult> Result = Compile(SourceBlob, Arguments);

	ComPtr<IDxcBlob> ShaderBlob;
	CHECK_RESULT(Result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&ShaderBlob), nullptr), "Failed to get shader blob")

	ComPtr<IDxcBlob> ReflectionBlob = nullptr;
	if (Description.bExtractReflection)
		CHECK_RESULT(Result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&ReflectionBlob), nullptr), "Failed to get reflection blob")

	FCompiledShader CompiledShader = {};
	CompiledShader.Stage = Description.Stage;
	CompiledShader.ShaderBlob = ShaderBlob;
	CompiledShader.ReflectionBlob = ReflectionBlob;
	return CompiledShader;
}

ComPtr<IDxcBlob> FShaderCompiler::CompileRootSignature(const std::string& FilePath, const EShaderConfiguration& Configuration) const
{
	FShaderCompilationDesc Description = {};
	Description.Stage = EShaderStage::RootSignature;
	Description.Configuration = Configuration;
	TVector<LPCWSTR> Arguments = GetArgumentsForDescription(Description);

	Arguments.push_back(L"-I");
	Arguments.push_back(ToWide(FDirectories::GetShadersDirectory().c_str()).c_str());

	ComPtr<IDxcBlobEncoding> SourceBlob = GetBufferFromFile(FilePath);
	ComPtr<IDxcResult> Result = Compile(SourceBlob, Arguments);

	ComPtr<IDxcBlob> RootSignatureBlob;
	CHECK_RESULT(Result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&RootSignatureBlob), nullptr), "Failed to get root signature blob")
	return RootSignatureBlob;
}

TVector<LPCWSTR> FShaderCompiler::GetArgumentsForDescription(const FShaderCompilationDesc& Description)
{
	TVector<LPCWSTR> Arguments;
	Arguments.push_back(L"-E");
	Arguments.push_back(GetEntrypointForStage(Description.Stage));

	Arguments.push_back(L"-T");
	Arguments.push_back(GetShaderModelTargetForStage(Description.Stage));

	Arguments.push_back(L"-Qstrip_debug");
	Arguments.push_back(L"-Qstrip_reflect");
	Arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);

	if (Description.Configuration == EShaderConfiguration::Debug)
		Arguments.push_back(DXC_ARG_DEBUG);
	else
		Arguments.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);

	for (FShaderMacro Macro : Description.Macros)
	{
		Arguments.push_back(L"-D");
		Arguments.push_back(ToWide(std::format("{}={}", Macro.Name, Macro.Value).c_str()).c_str());
	}
	return Arguments;
}

ComPtr<IDxcBlobEncoding> FShaderCompiler::GetBufferFromFile(const std::string& FilePath) const
{
	uint32 CodePage = DXC_CP_UTF8;
	ComPtr<IDxcBlobEncoding> SourceBlob;
	CHECK_RESULT(Library->CreateBlobFromFile(ToWide(FilePath.c_str()).c_str(), &CodePage, &SourceBlob), "Failed to create source blob from file")
	return SourceBlob;
}

ComPtr<IDxcResult> FShaderCompiler::Compile(const ComPtr<IDxcBlobEncoding>& Blob, TVector<LPCWSTR>& Arguments) const
{
	ComPtr<IDxcIncludeHandler> IncludeHandler;
	CHECK_RESULT(Library->CreateIncludeHandler(&IncludeHandler), "Failed to create include handler")

	DxcBuffer Buffer = {};
	Buffer.Ptr = Blob->GetBufferPointer();
	Buffer.Size = Blob->GetBufferSize();

	ComPtr<IDxcResult> Result;
	CHECK_RESULT(Compiler->Compile(&Buffer, Arguments.data(), static_cast<uint32>(Arguments.size()), IncludeHandler.Get(), IID_PPV_ARGS(&Result)),
	             "Failed to compile shader")

	ComPtr<IDxcBlobUtf8> ErrorBlob;
	CHECK_RESULT(Result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&ErrorBlob), nullptr), "Failed to get error blob")
	if (ErrorBlob && ErrorBlob->GetStringLength() > 0)
	{
		std::string ErrorString = std::string(ErrorBlob->GetStringPointer(), ErrorBlob->GetStringLength());
		RW_LOG_ERROR("Shader compilation failed with errors:");
		RW_LOG_ERROR("{}", ErrorString);
	}

	HRESULT Status;
	CHECK_RESULT(Result->GetStatus(&Status), "Failed to get shader compilation status")
	CHECK_RESULT(Status, "Failed to get shader compilation status")
	return Result;
}

const wchar_t* FShaderCompiler::GetEntrypointForStage(const EShaderStage Stage)
{
	switch (Stage)
	{
	case EShaderStage::Vertex: return L"VSMain";
	case EShaderStage::Pixel: return L"PSMain";
	case EShaderStage::RootSignature: return L"ROOT_SIGNATURE";
	case EShaderStage::None:
	default:
		RW_DEBUG_ASSERT(false, "Invalid shader stage")
		return L"Unknown";
	}
}

const wchar_t* FShaderCompiler::GetShaderModelTargetForStage(const EShaderStage Stage)
{
	switch (Stage)
	{
	case EShaderStage::Vertex: return L"vs_6_8";
	case EShaderStage::Pixel: return L"ps_6_8";
	case EShaderStage::RootSignature: return L"rootsig_1_1";
	case EShaderStage::None:
	default:
		RW_DEBUG_ASSERT(false, "Invalid shader stage")
		return L"Unknown";
	}
}
