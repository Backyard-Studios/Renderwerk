#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Memory/SharedPointer.h"

#include "Renderwerk/Core/Result.h"

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
private:
	[[nodiscard]] static FResult Initialize();
	static void Shutdown();

public:
	static void Fatal(FResultCode Code);
	static void Assertion(FAssertionData Data);

	[[nodiscard]] static std::string GetResultHandleDescription(HRESULT Result);

public:
	[[nodiscard]] INLINE static int32 GetExitCode() { return ExitCode; }
	INLINE static void SetExitCode(const int32 NewExitCode) { ExitCode = NewExitCode; }

	[[nodiscard]] INLINE static WNDCLASSEX GetWindowClass() { return WindowClass; }

protected:
	INLINE static int32 ExitCode = 0;
	INLINE static WNDCLASSEX WindowClass = {};

	friend int32 Launch();
	friend void Shutdown();
};

/**
 * Exception handler for the win32 platform.
 * This function is used in the SEH block to handle exceptions.
 * @param ExceptionInfo The exception information.
 * @return The exception handling result.
 */
ENGINE_API LONG ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo);
