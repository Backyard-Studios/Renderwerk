#pragma once

#include "Renderwerk/EngineMinimal.h"

struct RENDERWERK_API FModuleInfo
{
	FString Name;
};

class RENDERWERK_API IModule
{
public:
	IModule(const FString& InName)
		: ModuleInfo({InName})
	{
	}

	virtual ~IModule() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(IModule);

public:
	virtual void Start()
	{
	}

	virtual void Shutdown()
	{
	}

	NODISCARD virtual bool8 IsSupportingReloading() const
	{
		return true;
	}

public:
	NODISCARD FModuleInfo GetModuleInfo() const { return ModuleInfo; }

private:
	MAYBE_UNUSED FModuleInfo ModuleInfo = {};
};

#define MODULE_CREATE_FUNCTION_NAME CreateModule

/**
 * Declare a module class.
 * @note Only use this when you are compiling a separate module dll.
 * @param ModuleClass The module class to declare.
 */
#define DECLARE_MODULE(ModuleClass) \
	extern "C" DLL_EXPORT TSharedPtr<IModule> MODULE_CREATE_FUNCTION_NAME() \
	{ \
		return MakeShared<ModuleClass>(); \
	}

using FModuleCreateFn = TSharedPtr<IModule>(*)();
