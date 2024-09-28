#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

struct FAssertionData;

#define E_FATAL MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0001)
#define E_ASSERTION MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0002)

/**
 * Platform interface class.
 * This class is used to provide platform specific functionality.
 * You should inherit from this class and implement the platform specific functions.
 */
class ENGINE_API FPlatform
{
public:
	static void Initialize();
	static void Shutdown();

public:
	static void Fatal(const std::string& Reason);
	static void Assertion(FAssertionData Data);

	[[nodiscard]] static std::string GetResultHandleDescription(HRESULT Result);

	[[nodiscard]] static std::string GetCurrentWorkingDirectory();
	[[nodiscard]] static std::string AppendPath(const std::string& ParentPath, const std::string& PathToAppend);

public:
	[[nodiscard]] INLINE static int32 GetExitCode() { return ExitCode; }
	INLINE static void SetExitCode(const int32 NewExitCode) { ExitCode = NewExitCode; }

	[[nodiscard]] INLINE static WNDCLASSEX GetWindowClass() { return WindowClass; }

protected:
	INLINE static int32 ExitCode = 0;
	INLINE static WNDCLASSEX WindowClass = {};
};

/**
 * Exception handler for the win32 platform.
 * This function is used in the SEH block to handle exceptions.
 * @param ExceptionInfo The exception information.
 * @return The exception handling result.
 */
ENGINE_API LONG ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo);
