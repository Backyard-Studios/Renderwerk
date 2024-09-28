#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include <format>

struct ENGINE_API FAssertionData
{
	std::string Condition;
	std::string Message;

	FAssertionData(const std::string& Condition, const std::string& Message)
		: Condition(Condition), Message(Message)
	{
	}
};

/**
 * @brief Asserts that a condition is true. You should only use this in places that are critical for the program to work.
 * @param Condition The condition to assert.
 * @param Code The result code to return if the condition is false.
 * @param ... The message to display if the condition is false.
 */
#define RW_ASSERT_CRITICAL(Condition, ...) \
	{ \
		if (!(Condition)) [[unlikely]] \
			FPlatform::Assertion(FAssertionData(#Condition, std::format(__VA_ARGS__))); \
	}

#if RW_CONFIG_DEBUG || RW_CONFIG_DEVELOPMENT
/**
 * @brief Asserts that a condition is true. You should use this in places that are only relevant for debugging/development.
 * @param Condition The condition to assert.
 * @param Code The result code to return if the condition is false.
 * @param ... The message to display if the condition is false.
 */
#	define RW_DEBUG_ASSERT(Condition, ...) RW_ASSERT_CRITICAL(Condition, __VA_ARGS__)
#else
#	define RW_DEBUG_ASSERT(Condition, ...) {}
#endif
