#pragma once

#include <format>

#include "Renderwerk/Core/Compiler.h"
#include "Renderwerk/Platform/ExceptionHandling.h"

/**
 * @brief Asserts that a condition is true and reports an exception if it is not.
 * @param Condition The condition to check.
 */
#define ASSERT(Condition) \
	{ \
		if (!(Condition)) UNLIKELY \
		{ \
			if(IsDebuggerPresent()) UNLIKELY \
				DEBUG_BREAK(); \
			FExceptionHandling::Report(FExceptionInfo(FString(TEXT(#Condition)), TEXT(__FILE__), __LINE__, TEXT(__FUNCTION__))); \
		} \
	}

/**
 * @brief Asserts that a condition is true and reports an exception if it is not.
 * @param Condition The condition to check.
 * @param Result The result to report if the condition is false.
 */
#define ASSERTR(Condition, Result) \
	{ \
		if (!(Condition)) UNLIKELY \
		{ \
			if(IsDebuggerPresent()) UNLIKELY \
				DEBUG_BREAK(); \
			FExceptionHandling::Report(FExceptionInfo(FString(TEXT(#Condition)), TEXT(__FILE__), __LINE__, TEXT(__FUNCTION__), Result)); \
		} \
	}

/**
 * @brief Asserts that a condition is true and reports an exception if it is not using a formatted message.
 * @param Condition The condition to check.
 * @param Message The message to report if the condition is false.
 * @param ... The arguments to format the message with.
 */
#define ASSERTM(Condition, Message, ...) \
	{ \
		if (!(Condition)) UNLIKELY \
		{ \
			if(IsDebuggerPresent()) UNLIKELY \
				DEBUG_BREAK(); \
			FExceptionHandling::Report(FExceptionInfo(FString(std::format(TEXT(Message), __VA_ARGS__).c_str()), TEXT(__FILE__), __LINE__, TEXT(__FUNCTION__))); \
		} \
	}

/**
 * @brief Asserts that a condition is true and reports an exception if it is not using a formatted message.
 * @param Condition The condition to check.
 * @param Result The result to report if the condition is false.
 * @param Message The message to report if the condition is false.
 * @param ... The arguments to format the message with.
 */
#define ASSERTRM(Condition, Result, Message, ...) \
	{ \
		if (!(Condition)) UNLIKELY \
		{ \
			if(IsDebuggerPresent()) UNLIKELY \
				DEBUG_BREAK(); \
			FExceptionHandling::Report(FExceptionInfo(FString(std::format(TEXT(Message), __VA_ARGS__).c_str()), TEXT(__FILE__), __LINE__, TEXT(__FUNCTION__), Result)); \
		} \
	}

#if RW_CONFIG_DEBUG || RW_CONFIG_DEVELOPMENT
/**
 * @brief Asserts that a condition is true and reports an exception if it is not.
 * @note This version of the assert is only active in debug and development builds.
 * @param Condition The condition to check.
 */
#	define DEBUG_ASSERT(Condition) ASSERT(Condition)

/**
 * @brief Asserts that a condition is true and reports an exception if it is not.
 * @note This version of the assert is only active in debug and development builds.
 * @param Condition The condition to check.
 * @param Result The result to report if the condition is false.
 */
#	define DEBUG_ASSERTR(Condition, Result) ASSERTR(Condition, Result)

/**
 * @brief Asserts that a condition is true and reports an exception if it is not using a formatted message.
 * @note This version of the assert is only active in debug and development builds.
 * @param Condition The condition to check.
 * @param Message The message to report if the condition is false.
 * @param ... The arguments to format the message with.
 */
#	define DEBUG_ASSERTM(Condition, Message, ...) ASSERTM(Condition, Message, __VA_ARGS__)

/**
 * @brief Asserts that a condition is true and reports an exception if it is not using a formatted message.
 * @note This version of the assert is only active in debug and development builds.
 * @param Condition The condition to check.
 * @param Result The result to report if the condition is false.
 * @param Message The message to report if the condition is false.
 * @param ... The arguments to format the message with.
 */
#	define DEBUG_ASSERTRM(Condition, Result, Message, ...) ASSERTRM(Condition, Result, Message, __VA_ARGS__)
#else
#	define DEBUG_ASSERT(Condition) {}
#	define DEBUG_ASSERTR(Condition, Result) {}
#	define DEBUG_ASSERTM(Condition, Message, ...) {}
#	define DEBUG_ASSERTRM(Condition, Result, Message, ...) {}
#endif
