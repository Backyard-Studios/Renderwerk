#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Platform/Platform.h"

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
	void Initialize() override;
	void Shutdown() override;

public:
	void* Allocate(size_t Size, uint8 Alignment) override;
	void Free(void* Pointer, uint8 Alignment) override;
	void Zero(void* Pointer, size_t Size) override;
	void Copy(void* Destination, const void* Source, size_t Size) override;
	void Fill(void* Destination, uint8 Value, size_t Size) override;
	void Move(void* Destination, const void* Source, size_t Size) override;
	size_t GetAllocationSize(void* Pointer, uint8 Alignment) override;
};

/**
 * Safe way to get the win32 platform interface pointer.
 * This function is preferred over using `GetPlatform<FWin32Platform>()`.
 * @return The win32 platform interface.
 * @relatesalso GetPlatform
 */
ENGINE_API TSharedPtr<FWin32Platform> GetWin32Platform();
