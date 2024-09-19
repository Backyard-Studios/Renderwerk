#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Platform/Platform.h"

#include <format>

struct ENGINE_API FAssertionData
{
	std::string Condition;
	FResult Result;
	std::string Message;

	FAssertionData(const std::string& Condition, const FResult& Result, const std::string& Message)
		: Condition(Condition), Result(Result), Message(Message)
	{
	}
};

#define RW_ASSERT(Condition, Code, ...) \
	if (!(Condition)) \
	{ \
		GetPlatform()->Assertion(FAssertionData(#Condition, Code, std::format(__VA_ARGS__))); \
	} else {}

#if RW_CONFIG_DEBUG || RW_CONFIG_DEVELOPMENT
#	define RW_DEBUG_ASSERT(Condition, Code, ...) RW_ASSERT(Condition, Code, __VA_ARGS__)
#else
#	define RW_DEBUG_ASSERT(Condition, Code)
#endif
