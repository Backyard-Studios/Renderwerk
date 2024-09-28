#include "pch.h"

#include "Renderwerk/Platform/Directories.h"

std::string FDirectories::GetWorkingDirectory()
{
	return FPlatform::GetCurrentWorkingDirectory();
}

std::string FDirectories::GetAssetsDirectory()
{
	return FPlatform::AppendPath(GetWorkingDirectory(), "Assets");
}

std::string FDirectories::GetShadersDirectory()
{
	return FPlatform::AppendPath(GetAssetsDirectory(), "Shaders");
}

std::string FDirectories::GetShaderPath(const std::string& ShaderFileName)
{
	return FPlatform::AppendPath(GetShadersDirectory(), ShaderFileName);
}
