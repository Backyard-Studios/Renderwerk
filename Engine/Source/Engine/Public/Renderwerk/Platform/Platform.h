#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

struct RENDERWERK_API FProcessorInfo
{
	uint32 PhysicalCoreCount = 0;
	uint32 LogicalCoreCount = 0;
	bool bIs64Bit = false;
	FString Name = TEXT("");
};

struct RENDERWERK_API FMemoryInfo
{
	uint64 TotalPhysicalMemory = 0;
	uint64 FreePhysicalMemory = 0;
};

class RENDERWERK_API FPlatform
{
public:
	FPlatform();
	~FPlatform();

	DELETE_COPY_AND_MOVE(FPlatform);

public:
	NODISCARD HMODULE LoadDynamicLibrary(const FString& LibraryPath) const;
	NODISCARD void UnloadDynamicLibrary(HMODULE LibraryHandle) const;

	NODISCARD uint64 GetCurrentThreadId() const;

public:
	NODISCARD FProcessorInfo GetProcessorInfo() const { return ProcessorInfo; }
	NODISCARD FMemoryInfo GetMemoryInfo() const { return MemoryInfo; }

private:
	static FString QueryCPUName();
	static uint32 QueryPhysicalCoreCount();

private:
	FProcessorInfo ProcessorInfo = {};
	FMemoryInfo MemoryInfo = {};
};

/**
 * Global platform pointer. You should use GetPlatform() to safely access this pointer.
 * Please check the validity of the pointer before using it.
 */
RENDERWERK_API extern TSharedPtr<FPlatform> GPlatform;

/**
 * Checks the validity of the global platform pointer and returns it.
 * @return The global platform pointer.
 */
RENDERWERK_API TSharedPtr<FPlatform> GetPlatform();
