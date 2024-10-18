﻿#include "pch.h"

#include "Renderwerk/Platform/Platform.h"

TSharedPtr<FPlatform> GPlatform = nullptr;

FPlatform::FPlatform()
{
	SYSTEM_INFO SystemInfo;
	GetNativeSystemInfo(&SystemInfo);

	ProcessorInfo.PhysicalCoreCount = SystemInfo.dwNumberOfProcessors / 2;
	ProcessorInfo.LogicalCoreCount = SystemInfo.dwNumberOfProcessors;
	ProcessorInfo.bIs64Bit = SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
	ProcessorInfo.Name = QueryCPUName();
}

FPlatform::~FPlatform()
{
}

FString FPlatform::QueryCPUName()
{
	TArray<int32, 4> IntegerBuffer = {};
	constexpr size_t SizeofIntegerBuffer = sizeof(int) * IntegerBuffer.size();
	TArray<FAnsiChar, 64> CharBuffer = {};
	// https://learn.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=vs-2019
	constexpr TArray<uint32, 3> FunctionIds = {
		// Manufacturer
		0x8000'0002,
		// Model
		0x8000'0003,
		// Clockspeed
		0x8000'0004
	};
	FAnsiString Name = "";
	for (uint32 FunctionId : FunctionIds)
	{
		__cpuid(IntegerBuffer.data(), FunctionId);
		std::memcpy(CharBuffer.data(), IntegerBuffer.data(), SizeofIntegerBuffer);
		Name += FAnsiString(CharBuffer.data());
	}
#ifdef RW_USE_ANSI_STRINGS
	return Name;
#else
	return FStringUtils::ConvertToWideString(Name);
#endif
}
