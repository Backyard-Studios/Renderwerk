#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/DataTypes/String.h"
#include "Renderwerk/DataTypes/Types.h"

#include <Windows.h>

struct RENDERWERK_API FExceptionInfo
{
	FString Message = TEXT("");
	FString File = TEXT("");
	uint32 Line = 0;
	FString Function = TEXT("");
	HRESULT CustomCode = 0;

	FExceptionInfo(const FString& InMessage, const FString& InFile, const uint32 InLine, const FString& InFunction, const HRESULT& InCustomCode = 0)
		: Message(InMessage), File(InFile), Line(InLine), Function(InFunction), CustomCode(InCustomCode)
	{
		File = File.find_last_of(TEXT("\\")) != FString::npos ? File.substr(File.find_last_of(TEXT("\\")) + 1) : File;
	}
};

class RENDERWERK_API FExceptionHandling
{
public:
	static LONG Handler(EXCEPTION_POINTERS* ExceptionInfo);

	static void Report(const FExceptionInfo& ExceptionInfo);

	static FString GetResultString(HRESULT Result);

private:
};
