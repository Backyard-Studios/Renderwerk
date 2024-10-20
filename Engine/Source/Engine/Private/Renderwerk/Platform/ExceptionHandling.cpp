#include "pch.h"

#include "Renderwerk/Platform/ExceptionHandling.h"

#include <dbghelp.h>
#include <format>

#include "Renderwerk/Renderer/Graphics/GraphicsCommon.h"

#define CLR_EXCEPTION FORWARD(0xE0434352)
#define REPORT_EXCEPTION_CODE MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x1001)

using FWriteMiniDump = BOOL(WINAPI *)(HANDLE ProcessHandle, DWORD PID, HANDLE FileHandle,
                                      MINIDUMP_TYPE DumpType,
                                      PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                      PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                      PMINIDUMP_CALLBACK_INFORMATION CallbackParam
);

LONG FExceptionHandling::Handler(EXCEPTION_POINTERS* ExceptionInfo)
{
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == CLR_EXCEPTION)
		return EXCEPTION_CONTINUE_SEARCH;

	HMODULE DebugHelpModuleHandle = LoadLibrary(TEXT("dbghelp.dll"));
	if (FWriteMiniDump WriteMiniDump = reinterpret_cast<FWriteMiniDump>(GetProcAddress(DebugHelpModuleHandle, "MiniDumpWriteDump")))
	{
		HANDLE FileHandle = CreateFile(TEXT("CrashDump.dmp"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		MINIDUMP_TYPE DumpType = static_cast<MINIDUMP_TYPE>(MiniDumpWithFullMemoryInfo | MiniDumpFilterMemory | MiniDumpWithHandleData | MiniDumpWithThreadInfo |
			MiniDumpWithUnloadedModules);
		MINIDUMP_EXCEPTION_INFORMATION ExceptionParam;
		ExceptionParam.ThreadId = GetCurrentThreadId();
		ExceptionParam.ExceptionPointers = ExceptionInfo;
		ExceptionParam.ClientPointers = FALSE;
		WriteMiniDump(GetCurrentProcess(), GetCurrentProcessId(), FileHandle, DumpType, &ExceptionParam, nullptr, nullptr);
		CloseHandle(FileHandle);
	}
	FreeLibrary(DebugHelpModuleHandle);

	if (ExceptionInfo->ExceptionRecord->ExceptionCode == REPORT_EXCEPTION_CODE)
	{
		FExceptionInfo* Info = reinterpret_cast<FExceptionInfo*>(ExceptionInfo->ExceptionRecord->ExceptionInformation[0]);

		FString Message = TEXT("An exception was reported.\n\n");
		Message += Info->Message + TEXT("\n");
		if (Info->CustomCode != 0)
			Message += TEXT("\nCode: ") + GetResultString(Info->CustomCode);
#ifdef RW_USE_ANSI_STRINGS
		Message += TEXT("\nFile: ") + std::format(TEXT("{}:{}"), Info->File, std::to_string(Info->Line));
#else
		Message += TEXT("\nFile: ") + std::format(TEXT("{}:{}"), Info->File, std::to_wstring(Info->Line));
#endif
		Message += TEXT("\nFunction: ") + Info->Function;
		MessageBox(nullptr, Message.c_str(), TEXT("Renderwerk | Exception"), MB_OK | MB_ICONERROR);
	}
	else
	{
		FString Message = TEXT("An unhandled exception occurred.\n\n");
#ifdef RW_USE_ANSI_STRINGS
		Message += TEXT("Code: ") + std::to_string(ExceptionInfo->ExceptionRecord->ExceptionCode) + TEXT("\n");
#else
		Message += TEXT("Code: ") + std::to_wstring(ExceptionInfo->ExceptionRecord->ExceptionCode) + TEXT("\n");
#endif
		Message += TEXT("Description: ") + GetResultString(ExceptionInfo->ExceptionRecord->ExceptionCode) + TEXT("\n");
		MessageBox(nullptr, Message.c_str(), TEXT("Renderwerk | Exception"), MB_OK | MB_ICONERROR);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void FExceptionHandling::Report(const FExceptionInfo& ExceptionInfo)
{
	ULONG_PTR Parameters[] = {reinterpret_cast<ULONG_PTR>(&ExceptionInfo)};
	RaiseException(REPORT_EXCEPTION_CODE, EXCEPTION_NONCONTINUABLE, _countof(Parameters), Parameters);
}

FString FExceptionHandling::GetResultString(const HRESULT Result)
{
	FString ResultDescription;
	DWORD Facility = HRESULT_FACILITY(Result);
	if (Facility == FACILITY_ITF)
	{
		if (Result == CLR_EXCEPTION)
			ResultDescription = TEXT("CLR exception");
		else if (Result == REPORT_EXCEPTION_CODE)
			ResultDescription = TEXT("Exception reported");
		else
			ResultDescription = TEXT("Interface specific error");
	}
	else if (Facility == FACILITY_DIRECT3D12 || Facility == FACILITY_DXGI)
	{
		ResultDescription = D3D12ResultToString(Result);
	}
	else
	{
		LPWSTR MessageBuffer = nullptr;
		DWORD WrittenSize = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		                                  nullptr, Result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&MessageBuffer), 0, nullptr);
		if (WrittenSize)
		{
			if (MessageBuffer)
				ResultDescription = MessageBuffer;
			LocalFree(MessageBuffer);
		}
		else
		{
			ResultDescription = TEXT("Unknown error");
		}
	}
	return ResultDescription;
}
