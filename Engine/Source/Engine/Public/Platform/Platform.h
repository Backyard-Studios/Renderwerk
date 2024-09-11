﻿#pragma once

#include "Core/CoreDefinitions.h"

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
	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;

private:
};

/**
 * Global platform interface pointer. You shouldn't use this directly.
 * Use `GetPlatform()` instead.
 * @relatesalso GetPlatform()
 */
ENGINE_API extern IPlatform* GPlatform;

/**
 * Safe way to get the platform interface pointer.
 * This will check if the pointer is valid and return it.
 * @return The platform interface.
 */
ENGINE_API IPlatform* GetPlatform();

template <typename TPlatform>
TPlatform* GetPlatform() requires (std::is_base_of_v<IPlatform, TPlatform>)
{
	return static_cast<TPlatform*>(GetPlatform());
}
