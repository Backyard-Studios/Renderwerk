#include "pch.h"

#include "Renderwerk/Core/Result.h"

FResult::FResult()
	: Code(RW_RESULT_CODE_SUCCESS)
{
}

FResult::FResult(const FResultCode InCode)
	: Code(InCode)
{
}

FResult::~FResult()
{
}

bool FResult::IsSuccess() const
{
	return GetSeverity() == RW_RESULT_SEVERITY_SUCCESS;
}

bool FResult::IsError() const
{
	return GetSeverity() == RW_RESULT_SEVERITY_ERROR;
}

FResultCode FResult::GetCode() const
{
	return Code;
}

uint32 FResult::GetSeverity() const
{
	return RW_RESULT_GET_SEVERITY(Code);
}

uint32 FResult::GetErrorCode() const
{
	return RW_RESULT_GET_ERROR_CODE(Code);
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

TMap<FResultCode, std::string> ResultDescriptions;

void RegisterResultDescription(const FResultCode Code, std::string Description)
{
	ResultDescriptions.insert_or_assign(Code, Description);
}

void RegisterDefaultResultDescriptions()
{
	RegisterResultDescription(RW_RESULT_CODE_SUCCESS, "Success");
	RegisterResultDescription(RW_RESULT_CODE_FAIL, "Fail");
	RegisterResultDescription(RW_RESULT_CODE_UNKNOWN_ERROR, "Unknown error");
}

std::string ToString(const FResultCode Code)
{
	if (ResultDescriptions.contains(Code))
		return ResultDescriptions.at(Code);
	return "Unknown error";
}

std::string ToString(const FResult& Result)
{
	return ToString(Result.GetCode());
}
