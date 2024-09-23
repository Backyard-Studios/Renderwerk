#include "pch.h"

#include "Renderwerk/Core/Result.h"

FResult::FResult()
	: Code(RESULT_SUCCESS)
{
}

FResult::FResult(const EResultCode InCode, const std::string& InReason)
	: Code(InCode), Reason(InReason.empty() ? ToString(InCode) : InReason)
{
}

FResult::~FResult()
{
}

bool FResult::IsSuccess() const
{
	return GetCode() == RESULT_SUCCESS;
}

bool FResult::IsError() const
{
	return !IsSuccess();
}

EResultCode FResult::GetCode() const
{
	return Code;
}

std::string FResult::GetReason() const
{
	return Reason;
}

bool operator==(const FResult& Lhs, const FResult& Rhs)
{
	return Lhs.GetCode() == Rhs.GetCode();
}

bool operator!=(const FResult& Lhs, const FResult& Rhs)
{
	return !(Lhs == Rhs);
}

bool FResult::operator()() const
{
	return IsSuccess();
}

std::string ToString(const EResultCode Code)
{
	switch (Code)
	{
	case RESULT_SUCCESS: return "Success";
	case RESULT_FAILED: return "Failed";
	case RESULT_VULKAN_FAILED_RESULT: return "Vulkan failed result";
	default:
		return "Unknown error";
	}
}

std::string ToString(const FResult& Result)
{
	return ToString(Result.GetCode());
}
