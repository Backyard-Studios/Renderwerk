#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"

#include <string>

using FResultCode = uint64;

#define RW_MAKE_RESULT_CODE(Severity, ErrorCode) ((Severity << 31) | ErrorCode)
#define RW_RESULT_GET_SEVERITY(ResultCode) ((ResultCode >> 31) & 0x1)
#define RW_RESULT_GET_ERROR_CODE(ResultCode) (ResultCode & 0xFFFF)

#define RW_DEFINE_ERROR_CODE(ErrorCode) ErrorCode

#pragma region Severity

#define RW_RESULT_SEVERITY_SUCCESS RW_DEFINE_ERROR_CODE(0)
#define RW_RESULT_SEVERITY_ERROR RW_DEFINE_ERROR_CODE(1)

#pragma endregion

#pragma region Codes

#define RW_RESULT_CODE_SUCCESS RW_MAKE_RESULT_CODE(RW_RESULT_SEVERITY_SUCCESS, 0)

#define RW_RESULT_CODE_FAIL RW_MAKE_RESULT_CODE(RW_RESULT_SEVERITY_ERROR, 1)
#define RW_RESULT_CODE_UNKNOWN_ERROR RW_MAKE_RESULT_CODE(RW_RESULT_SEVERITY_ERROR, 2)

#pragma endregion

/**
 * @brief Represents the result of an operation.
 */
class ENGINE_API FResult
{
public:
	FResult();
	FResult(FResultCode InCode);
	~FResult();

	DEFINE_DEFAULT_COPY_AND_MOVE(FResult)

public:
	[[nodiscard]] bool IsSuccess() const;
	[[nodiscard]] bool IsError() const;

public:
	/**
	 * @brief Returns the whole result code. Including the severity and the error code.
	 * @return The whole result code.
	 */
	[[nodiscard]] FResultCode GetCode() const;

	/**
	 * @brief Returns the severity of the result.
	 * @return The severity of the result.
	 */
	[[nodiscard]] uint32 GetSeverity() const;

	/**
	 * @brief Returns the error code of the result.
	 * @return The error code of the result.
	 */
	[[nodiscard]] uint32 GetErrorCode() const;

public:
	friend bool operator==(const FResult& Lhs, const FResult& Rhs);
	friend bool operator!=(const FResult& Lhs, const FResult& Rhs);

	bool operator()() const;

private:
	FResultCode Code;
};

ENGINE_API void RegisterResultDescription(FResultCode Code, std::string Description);
ENGINE_API void RegisterDefaultResultDescriptions();

ENGINE_API std::string ToString(FResultCode Code);
ENGINE_API std::string ToString(const FResult& Result);
