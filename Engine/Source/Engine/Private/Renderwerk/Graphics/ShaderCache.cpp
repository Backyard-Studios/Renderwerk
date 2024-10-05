#include "pch.h"

#include "Renderwerk/Graphics/ShaderCache.h"

TMap<std::string, FCompiledShader> FShaderCache::ShaderCache;

void FShaderCache::Register(const std::string& Name, const FCompiledShader& Shader)
{
	ShaderCache.insert_or_assign(Name, Shader);
}

void FShaderCache::Remove(const std::string& Name)
{
	RW_DEBUG_ASSERT(ShaderCache.contains(Name), "Shader not found in cache")
	ShaderCache.erase(Name);
}

void FShaderCache::Clear()
{
	ShaderCache.clear();
}

bool8 FShaderCache::Exists(const std::string& Name)
{
	return ShaderCache.contains(Name);
}

FCompiledShader FShaderCache::Get(const std::string& Name)
{
	RW_DEBUG_ASSERT(ShaderCache.contains(Name), "Shader not found in cache")
	return ShaderCache.at(Name);
}
