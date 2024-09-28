#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

class ENGINE_API FDirectories
{
public:
	static std::string GetWorkingDirectory();
	static std::string GetAssetsDirectory();

	static std::string GetShadersDirectory();
	static std::string GetShaderPath(const std::string& ShaderFileName);

private:
};
