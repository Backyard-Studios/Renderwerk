#include "pch.h"

#include "Renderwerk/Platform/Win32/Win32Platform.h"

#include "Renderwerk/Platform/Guid.h"

#include <dbghelp.h>
#include <objbase.h>
#include <Windows.h>

#include "Renderwerk/Platform/Win32/Win32Window.h"

#define CLR_EXCEPTION FORWARD(0xE0434352)

using FWriteMiniDump = BOOL(WINAPI *)(HANDLE ProcessHandle, DWORD PID, HANDLE FileHandle,
                                      MINIDUMP_TYPE DumpType,
                                      PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                      PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                      PMINIDUMP_CALLBACK_INFORMATION CallbackParam
);

LRESULT CALLBACK WindowProcess(const HWND WindowHandle, const UINT Message, const WPARAM WParam, const LPARAM LParam)
{
	if (FWin32Window* Window = reinterpret_cast<FWin32Window*>(GetWindowLongPtr(WindowHandle, GWLP_USERDATA)))
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

FWin32Platform::FWin32Platform() = default;

FWin32Platform::~FWin32Platform() = default;

FResult FWin32Platform::Initialize()
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

	return RW_RESULT_CODE_SUCCESS;
}

void FWin32Platform::Shutdown()
{
	UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
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

FGuid NewGuid()
{
	GUID Guid;
	CoCreateGuid(&Guid);

	TVector<uint8> Bytes =
	{
		static_cast<uint8>((Guid.Data1 >> 24) & 0xFF),
		static_cast<uint8>((Guid.Data1 >> 16) & 0xFF),
		static_cast<uint8>((Guid.Data1 >> 8) & 0xFF),
		static_cast<uint8>((Guid.Data1) & 0xff),

		static_cast<uint8>((Guid.Data2 >> 8) & 0xFF),
		static_cast<uint8>((Guid.Data2) & 0xff),

		static_cast<uint8>((Guid.Data3 >> 8) & 0xFF),
		static_cast<uint8>((Guid.Data3) & 0xFF),

		(Guid.Data4[0]),
		(Guid.Data4[1]),
		(Guid.Data4[2]),
		(Guid.Data4[3]),
		(Guid.Data4[4]),
		(Guid.Data4[5]),
		(Guid.Data4[6]),
		(Guid.Data4[7])
	};
	return FGuid{
		Bytes
	};
}
