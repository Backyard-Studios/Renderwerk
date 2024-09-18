#include "pch.h"

#include "Renderwerk/Platform/Win32/Win32Platform.h"

#include <Windows.h>

#include <dbghelp.h>

#define CLR_EXCEPTION FORWARD(0xE0434352)

using FWriteMiniDump = BOOL(WINAPI *)(HANDLE ProcessHandle, DWORD PID, HANDLE FileHandle,
                                      MINIDUMP_TYPE DumpType,
                                      PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                      PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                      PMINIDUMP_CALLBACK_INFORMATION CallbackParam
);

FWin32Platform::FWin32Platform() = default;

FWin32Platform::~FWin32Platform() = default;

FResult FWin32Platform::Initialize()
{
	SetUnhandledExceptionFilter(ExceptionHandler);
	return RW_RESULT_CODE_SUCCESS;
}

void FWin32Platform::Shutdown()
{
}

void FWin32Platform::Fatal(const FResultCode Code)
{
	ULONG_PTR Parameters[] = {Code};
	RaiseException(E_FATAL, EXCEPTION_NONCONTINUABLE, _countof(Parameters), Parameters);
}

void FWin32Platform::Assertion(FAssertionData Data)
{
	ULONG_PTR Parameters[] = {reinterpret_cast<ULONG_PTR>(&Data)};
	RaiseException(E_ASSERTION, EXCEPTION_NONCONTINUABLE, _countof(Parameters), Parameters);
}

std::string FWin32Platform::GetResultHandleDescription(const HRESULT Result)
{
	std::string ResultDescription;
	DWORD Facility = HRESULT_FACILITY(Result);
	if (Facility == FACILITY_ITF)
	{
		ResultDescription = "Interface specific error";
	}
	else
	{
		LPSTR MessageBuffer = nullptr;
		DWORD WrittenSize = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		                                   nullptr, Result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&MessageBuffer), 0, nullptr);
		if (WrittenSize)
		{
			if (MessageBuffer)
				ResultDescription = MessageBuffer;
			LocalFree(MessageBuffer);
		}
		else
		{
			ResultDescription = "Unknown error";
		}
	}
	return ResultDescription;
}

TSharedPointer<FWin32Platform> GetWin32Platform()
{
	return GetPlatform<FWin32Platform>();
}

LONG ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
	IPlatform::SetExitCode(1);

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

	std::string MessageBoxText = "";
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == E_ASSERTION)
	{
		FAssertionData* Data = reinterpret_cast<FAssertionData*>(ExceptionInfo->ExceptionRecord->ExceptionInformation[0]);
		MessageBoxText += "An assertion failed. A crash dump has been saved to \"CrashDump.dmp\".";
		MessageBoxText += "\n\n" + Data->Message;
		MessageBoxText += "\n\nCode: " + std::to_string(Data->Code);
		MessageBoxText += "\nDescription: " + ToString(Data->Code);
		MessageBoxText += "\nCondition: " + Data->Condition;
	}
	else if (ExceptionInfo->ExceptionRecord->ExceptionCode == E_FATAL)
	{
		FResultCode Code = ExceptionInfo->ExceptionRecord->ExceptionInformation[0];
		MessageBoxText += "An unhandled exception occurred. A crash dump has been saved to \"CrashDump.dmp\".";
		MessageBoxText += "\n\nCode: " + std::to_string(Code);
		MessageBoxText += "\nDescription: " + ToString(Code);
	}
	else
	{
		MessageBoxText += "An unhandled exception occurred. A crash dump has been saved to \"CrashDump.dmp\".";
		MessageBoxText += "\n\nCode: " + std::to_string(ExceptionInfo->ExceptionRecord->ExceptionCode);
		MessageBoxText += "\nDescription: " + GetWin32Platform()->GetResultHandleDescription(ExceptionInfo->ExceptionRecord->ExceptionCode);
	}
	MessageBoxA(nullptr, MessageBoxText.c_str(), "Renderwerk | ExceptionHandler", MB_OK | MB_ICONERROR);

	return EXCEPTION_EXECUTE_HANDLER;
}
