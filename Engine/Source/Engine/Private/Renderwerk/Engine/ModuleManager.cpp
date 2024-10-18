#include "pch.h"

#include "Renderwerk/Engine/ModuleManager.h"

FModuleManager::FModuleManager()
{
}

FModuleManager::~FModuleManager()
{
	UnloadAllModules();
}

void FModuleManager::LoadModule(const TSharedPtr<IModule>& Module)
{
	DEBUG_ASSERTM(Module, "Module is nullptr");
	Module->Start();
	Modules.insert({Module->GetModuleInfo().Name, Module});
}

void FModuleManager::LoadModule(const FString& ModuleName)
{
	DEBUG_ASSERTM(!ModuleName.empty(), "Module name is empty");
	HMODULE ModuleHandle = GetPlatform()->LoadDynamicLibrary(ModuleName);
	FModuleCreateFn CreateModule = reinterpret_cast<FModuleCreateFn>(GetProcAddress(ModuleHandle, STRINGIFY(MODULE_CREATE_FUNCTION_NAME)));

	TSharedPtr<IModule> Module = CreateModule();
	Module->Start();
	Modules.insert({Module->GetModuleInfo().Name, Module});
	ModuleLibraryHandles.insert({Module->GetModuleInfo().Name, ModuleHandle});
}

void FModuleManager::UnloadModule(const TSharedPtr<IModule>& Module)
{
	DEBUG_ASSERTM(Module, "Module is nullptr");
	Module->Shutdown();
	if (IsDynamicallyLoaded(Module))
		GetPlatform()->UnloadDynamicLibrary(ModuleLibraryHandles.at(Module->GetModuleInfo().Name));
	EraseQueue.push(Module->GetModuleInfo().Name);
}

void FModuleManager::UnloadModule(const FString& ModuleName)
{
	UnloadModule(GetModule(ModuleName));
}

void FModuleManager::UnloadAllModules()
{
	for (const TSharedPtr<IModule>& Module : Modules | std::views::values)
		UnloadModule(Module);
	FlushEraseQueue();
	Modules.clear();
	ModuleLibraryHandles.clear();
}

TSharedPtr<IModule> FModuleManager::GetModule(const FString& ModuleName) const
{
	DEBUG_ASSERTM(!ModuleName.empty(), "Module name is empty");
	ASSERTM(Modules.contains(ModuleName), "Module not found: {}", ModuleName);
	return Modules.at(ModuleName);
}

bool8 FModuleManager::IsDynamicallyLoaded(const FString& ModuleName) const
{
	DEBUG_ASSERTM(!ModuleName.empty(), "Module name is empty");
	return ModuleLibraryHandles.contains(ModuleName);
}

bool8 FModuleManager::IsDynamicallyLoaded(const TSharedPtr<IModule>& Module) const
{
	DEBUG_ASSERTM(Module, "Module is nullptr");
	return IsDynamicallyLoaded(Module->GetModuleInfo().Name);
}

void FModuleManager::FlushEraseQueue()
{
	while (!EraseQueue.empty())
	{
		FString ModuleName = EraseQueue.front();
		EraseQueue.pop();
		Modules.erase(ModuleName);
		if (ModuleLibraryHandles.contains(ModuleName))
			ModuleLibraryHandles.erase(ModuleName);
	}
}
