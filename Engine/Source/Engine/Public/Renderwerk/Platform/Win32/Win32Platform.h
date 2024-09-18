#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Platform/Platform.h"

#define E_FATAL MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0001)
#define E_ASSERTION MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0002)

/**
 * Win32 platform interface class.
 * This class is used to provide win32 specific functionality.
 */
class ENGINE_API FWin32Platform : public IPlatform
{
public:
	FWin32Platform();
	~FWin32Platform() override;

	DELETE_COPY_AND_MOVE(FWin32Platform)

private:
	FResult Initialize() override;
	void Shutdown() override;

public:
	void Fatal(FResultCode Code) override;
	void Assertion(FAssertionData Data) override;

	[[nodiscard]] static std::string GetResultHandleDescription(HRESULT Result);

public:
	[[nodiscard]] WNDCLASSEX GetWindowClass() const { return WindowClass; }

private:
	WNDCLASSEX WindowClass = {};
};

/**
 * Safe way to get the win32 platform interface pointer.
 * This function is preferred over using `GetPlatform<FWin32Platform>()`.
 * @return The win32 platform interface.
 * @relatesalso GetPlatform
 */
ENGINE_API TSharedPointer<FWin32Platform> GetWin32Platform();

/**
 * Exception handler for the win32 platform.
 * This function is used in the SEH block to handle exceptions.
 * @param ExceptionInfo The exception information.
 * @return The exception handling result.
 */
ENGINE_API LONG ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo);
