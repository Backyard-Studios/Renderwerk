#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Memory/SharedPointer.h"

#include "Renderwerk/Core/Result.h"

#include <type_traits>


/**
 * Platform interface class.
 * This class is used to provide platform specific functionality.
 * You should inherit from this class and implement the platform specific functions.
 */
class ENGINE_API IPlatform
{
public:
	IPlatform() = default;
	virtual ~IPlatform() = default;

	DELETE_COPY_AND_MOVE(IPlatform)

private:
	virtual FResult Initialize() = 0;
	virtual void Shutdown() = 0;

public:
	[[nodiscard]] INLINE static int32 GetExitCode() { return ExitCode; }
	static void SetExitCode(const int32 NewExitCode) { ExitCode = NewExitCode; }

protected:
	INLINE static int32 ExitCode = 0;

	friend int32 Launch();
	friend void Shutdown();
};

/**
 * Global platform interface pointer. You shouldn't use this directly.
 * Use `GetPlatform()` instead.
 * @relatesalso GetPlatform
 */
ENGINE_API extern TSharedPointer<IPlatform> GPlatform;

/**
 * Safe way to get the platform interface pointer.
 * This will check if the pointer is valid and return it.
 * @return The platform interface.
 */
ENGINE_API TSharedPointer<IPlatform> GetPlatform();

template <typename TPlatform>
TSharedPointer<TPlatform> GetPlatform() requires (std::is_base_of_v<IPlatform, TPlatform>)
{
	return GetPlatform().As<TPlatform>();
}
