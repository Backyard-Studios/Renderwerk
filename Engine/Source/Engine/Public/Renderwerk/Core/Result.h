#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"

#include <string>

enum ENGINE_API EResultCode : uint32
{
	RESULT_SUCCESS = 0,
	RESULT_FAILED,
};

/**
 * @brief Represents the result of an operation.
 */
struct ENGINE_API FResult
{
public:
	FResult();
	FResult(EResultCode InCode, const std::string& InReason = "");
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
	[[nodiscard]] EResultCode GetCode() const;

	/**
	 * @brief Returns the reason of the result.
	 * @return The reason of the result.
	 */
	[[nodiscard]] std::string GetReason() const;

	friend bool operator==(const FResult& Lhs, const FResult& Rhs);
	friend bool operator!=(const FResult& Lhs, const FResult& Rhs);

	bool operator()() const;

private:
	EResultCode Code;
	std::string Reason;
};

ENGINE_API std::string ToString(EResultCode Code);
ENGINE_API std::string ToString(const FResult& Result);
