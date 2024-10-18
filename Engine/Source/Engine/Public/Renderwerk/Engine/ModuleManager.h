#pragma once

#include "Renderwerk/EngineMinimal.h"
#include "Renderwerk/Engine/Module.h"

class RENDERWERK_API FModuleManager
{
public:
	FModuleManager();
	~FModuleManager();

	DELETE_COPY_AND_MOVE(FModuleManager);

public:
	/**
	 * @brief Load an internal module from an instance.
	 * @param Module The module that will be loaded.
	 */
	void LoadModule(const TSharedPtr<IModule>& Module);

	/**
	 * @brief Load a module from a DLL by name.
	 * @param ModuleName The name of the module that will be loaded.
	 */
	void LoadModule(const FString& ModuleName);

	/**
	 * @brief Unload a module from an instance.
	 * @note You should call `FlushEraseQueue` after calling this function.
	 * @param Module The module that will be unloaded.
	 */
	void UnloadModule(const TSharedPtr<IModule>& Module);

	/**
	 * @brief Unload a module from a DLL by name.
	 * @note You should call `FlushEraseQueue` after calling this function.
	 * @param ModuleName The name of the module that will be unloaded.
	 */
	void UnloadModule(const FString& ModuleName);

	/**
	 * @brief Unloads all modules.
	 */
	void UnloadAllModules();

	/**
	 * @brief Returns a module safely by name.
	 * @param ModuleName The name of the module that will be returned.
	 */
	NODISCARD TSharedPtr<IModule> GetModule(const FString& ModuleName) const;

	/**
	 * @brief Returns a module safely by name.
	 * @tparam T The type of the module that will be returned.
	 * @param ModuleName The name of the module that will be returned.
	 */
	template <typename T>
	NODISCARD TSharedPtr<T> GetModule(const FString& ModuleName) const
	{
		return std::static_pointer_cast<T>(GetModule(ModuleName));
	}

	/**
	 * @brief Checks if a module is a loaded DLL or not.
	 * @param ModuleName The name of the module that will be checked.
	 */
	NODISCARD bool8 IsDynamicallyLoaded(const FString& ModuleName) const;

	/**
	 * @brief Checks if a module is a loaded DLL or not.
	 * @param Module The module that will be checked.
	 */
	NODISCARD bool8 IsDynamicallyLoaded(const TSharedPtr<IModule>& Module) const;

	/**
	 * @brief Arases all modules that are unloaded.
	 */
	void FlushEraseQueue();

private:
	TMap<FString, TSharedPtr<IModule>> Modules;
	TMap<FString, HMODULE> ModuleLibraryHandles;

	TQueue<FString> EraseQueue;
};
