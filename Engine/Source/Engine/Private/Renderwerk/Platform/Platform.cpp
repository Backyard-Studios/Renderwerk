#include "pch.h"

#include "Renderwerk/Platform/Platform.h"

#include <cassert>
#include <dbghelp.h>
#include <shlwapi.h>

#include "Renderwerk/Platform/Window.h"

#define CLR_EXCEPTION FORWARD(0xE0434352)

using FWriteMiniDump = BOOL(WINAPI *)(HANDLE ProcessHandle, DWORD PID, HANDLE FileHandle,
                                      MINIDUMP_TYPE DumpType,
                                      PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                      PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                      PMINIDUMP_CALLBACK_INFORMATION CallbackParam
);

LRESULT CALLBACK WindowProcess(const HWND WindowHandle, const UINT Message, const WPARAM WParam, const LPARAM LParam)
{
	if (FWindow* Window = reinterpret_cast<FWindow*>(GetWindowLongPtr(WindowHandle, GWLP_USERDATA)))
	{
		return Window->WindowProcess(WindowHandle, Message, WParam, LParam);
	}
	if (Message == WM_CREATE)
	{
		CREATESTRUCT* CreateStruct = reinterpret_cast<CREATESTRUCT*>(LParam);
		SetWindowLongPtr(WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(CreateStruct->lpCreateParams));
	}
	return DefWindowProc(WindowHandle, Message, WParam, LParam);
}

void FPlatform::Initialize()
{
	SetUnhandledExceptionFilter(ExceptionHandler);

	WindowClass = {};
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProcess;
	WindowClass.hInstance = GetModuleHandle(nullptr);
	WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	WindowClass.hbrBackground = nullptr;
	WindowClass.lpszClassName = TEXT("RenderwerkWindowClass");
	WindowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	WindowClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	RegisterClassEx(&WindowClass);

	RW_LOG_INFO("Win32 platform initialized");
}

void FPlatform::Shutdown()
{
	UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
}

void FPlatform::Fatal(const std::string& Reason)
{
	ULONG_PTR Parameters[] = {reinterpret_cast<ULONG_PTR>(&Reason)};
	RaiseException(E_FATAL, EXCEPTION_NONCONTINUABLE, _countof(Parameters), Parameters);
}

void FPlatform::Assertion(FAssertionData Data)
{
	ULONG_PTR Parameters[] = {reinterpret_cast<ULONG_PTR>(&Data)};
	RaiseException(E_ASSERTION, EXCEPTION_NONCONTINUABLE, _countof(Parameters), Parameters);
}

std::string FPlatform::GetResultHandleDescription(const HRESULT Result)
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

std::string FPlatform::GetCurrentWorkingDirectory()
{
	char Buffer[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, Buffer);
	return Buffer;
}

std::string FPlatform::AppendPath(const std::string& ParentPath, const std::string& PathToAppend)
{
	char Buffer[MAX_PATH];
	PathCombineA(Buffer, ParentPath.c_str(), PathToAppend.c_str());
	return Buffer;
}

LONG ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
	FPlatform::SetExitCode(1);

	if (ExceptionInfo->ExceptionRecord->ExceptionCode == CLR_EXCEPTION)
	{
		RW_LOG_ERROR("Unhandled CLR exception occurred. Forwarding exception handler");
		return EXCEPTION_CONTINUE_SEARCH;
	}

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
		MessageBoxText += "\nCondition: " + Data->Condition;
		RW_LOG_CRITICAL("An assertion failed. A crash dump has been saved to \"CrashDump.dmp\".");
		RW_LOG_CRITICAL("Condition: {0}", Data->Condition);
	}
	else if (ExceptionInfo->ExceptionRecord->ExceptionCode == E_FATAL)
	{
		std::string* Reason = reinterpret_cast<std::string*>(ExceptionInfo->ExceptionRecord->ExceptionInformation[0]);
		MessageBoxText += "An unhandled exception occurred. A crash dump has been saved to \"CrashDump.dmp\".";
		MessageBoxText += "\n\n" + *Reason;
		RW_LOG_CRITICAL("An unhandled exception occurred. A crash dump has been saved to \"CrashDump.dmp\".");
		RW_LOG_CRITICAL("{0}", *Reason);
	}
	else
	{
		MessageBoxText += "An unhandled exception occurred. A crash dump has been saved to \"CrashDump.dmp\".";
		MessageBoxText += "\n\nCode: " + std::to_string(ExceptionInfo->ExceptionRecord->ExceptionCode);
		MessageBoxText += "\nDescription: " + FPlatform::GetResultHandleDescription(ExceptionInfo->ExceptionRecord->ExceptionCode);
		RW_LOG_CRITICAL("An unhandled exception occurred. A crash dump has been saved to \"CrashDump.dmp\".");
		RW_LOG_CRITICAL("Code: {0}", ExceptionInfo->ExceptionRecord->ExceptionCode);
		RW_LOG_CRITICAL("Description: {0}", FPlatform::GetResultHandleDescription(ExceptionInfo->ExceptionRecord->ExceptionCode));
	}
	MessageBoxA(nullptr, MessageBoxText.c_str(), "Renderwerk | ExceptionHandler", MB_OK | MB_ICONERROR);

	return EXCEPTION_EXECUTE_HANDLER;
}
