#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Memory/SmartPointers.h"

struct RENDERWERK_API FProcessorInfo
{
	uint32 PhysicalCoreCount = 0;
	uint32 LogicalCoreCount = 0;
	bool bIs64Bit = false;
	FString Name = TEXT("");
};

class RENDERWERK_API FPlatform
{
public:
	FPlatform();
	~FPlatform();

	DELETE_COPY_AND_MOVE(FPlatform);

public:
	NODISCARD FProcessorInfo GetProcessorInfo() const { return ProcessorInfo; }

private:
	static FString QueryCPUName();

private:
	FProcessorInfo ProcessorInfo = {};
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
