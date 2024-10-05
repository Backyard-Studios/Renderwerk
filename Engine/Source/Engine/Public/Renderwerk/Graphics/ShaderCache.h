#pragma once

#include "ShaderCompiler.h"

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"

/**
 * @brief Shader cache for storing compiled shaders.
 * TODO: Implement a way to save and load compiled shaders from disk.
 */
class ENGINE_API FShaderCache
{
public:
	static void Register(const std::string& Name, const FCompiledShader& Shader);
	static void Remove(const std::string& Name);
	static void Clear();

	[[nodiscard]] static bool8 Exists(const std::string& Name);
	[[nodiscard]] static FCompiledShader Get(const std::string& Name);

private:
	static TMap<std::string, FCompiledShader> ShaderCache;
};
