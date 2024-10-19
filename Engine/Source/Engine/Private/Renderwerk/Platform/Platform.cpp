#include "pch.h"
// ReSharper disable CppMemberFunctionMayBeStatic

#include "Renderwerk/Platform/Platform.h"

TSharedPtr<FPlatform> GPlatform = nullptr;

FPlatform::FPlatform()
{
	SYSTEM_INFO SystemInfo = {};
	GetNativeSystemInfo(&SystemInfo);

	ProcessorInfo.PhysicalCoreCount = QueryPhysicalCoreCount();
	ProcessorInfo.LogicalCoreCount = SystemInfo.dwNumberOfProcessors;
	ProcessorInfo.bIs64Bit = SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
	ProcessorInfo.Name = QueryCPUName();

	MEMORYSTATUSEX MemoryStatus = {};
	MemoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&MemoryStatus);

	MemoryInfo.TotalPhysicalMemory = MemoryStatus.ullTotalPhys;
	MemoryInfo.FreePhysicalMemory = MemoryStatus.ullAvailPhys;
}

FPlatform::~FPlatform()
{
}

HMODULE FPlatform::LoadDynamicLibrary(const FString& LibraryPath) const
{
	return LoadLibrary(LibraryPath.c_str());
}

void FPlatform::UnloadDynamicLibrary(const HMODULE LibraryHandle) const
{
	FreeLibrary(LibraryHandle);
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

uint32 FPlatform::QueryPhysicalCoreCount()
{
	DWORD BufferSize = 0;
	GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &BufferSize);
	ASSERT(GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	std::byte* Buffer = FMemory::NewArray<std::byte>(BufferSize);
	const PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX Info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(Buffer);
	bool8 Result = GetLogicalProcessorInformationEx(RelationProcessorCore, Info, &BufferSize);
	ASSERT(Result)

	uint32 PhysicalCoreCount = 0;
	size64 Offset = 0;
	do
	{
		const PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX CurrentInfo = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(Buffer + Offset);
		Offset += CurrentInfo->Size;
		++PhysicalCoreCount;
	}
	while (Offset < BufferSize);
	return PhysicalCoreCount;
}

TSharedPtr<FPlatform> GetPlatform()
{
	DEBUG_ASSERTM(GPlatform, "Global platform pointer is null");
	return GPlatform;
}
