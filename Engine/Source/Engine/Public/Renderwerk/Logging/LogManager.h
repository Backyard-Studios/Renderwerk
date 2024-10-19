#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Memory/SmartPointers.h"

#include "spdlog/async_logger.h"
#include "spdlog/spdlog.h"

DECLARE_LOG_CATEGORY(LogDefault, Trace);

class RENDERWERK_API FLogManager
{
public:
	using FLogger = spdlog::logger;
	using FAsyncLogger = spdlog::async_logger;
	using ENativeVerbosity = spdlog::level::level_enum;

public:
	/**
	 * @brief Register a new log category.
	 * @note If the category is already registered, this function does nothing.
	 * @param Category The category to register.
	 */
	static void RegisterLogCategory(const ILogCategory& Category);

	/**
	 * @brief Get the logger for the specified category.
	 * @note If the logger does not exist, it will be created.
	 * @param Category The category to get the logger for.
	 * @return The logger for the specified category.
	 */
	static TSharedPtr<FLogger> GetLogger(const ILogCategory& Category);

	/**
	 * @brief Converts the verbosity enum value to the native enum.
	 * @param Verbosity The verbosity enum value to convert.
	 * @return The native enum value.
	 */
	static ENativeVerbosity ConvertVerbosity(const ELogVerbosity& Verbosity);

private:
	static void Initialize();
	static void Shutdown();

	static ENativeVerbosity ConvertVerbosity(uint32 Verbosity);

	friend RENDERWERK_API int32 LaunchRenderwerk();
};

#define RW_LOG(Category, Verbosity, Format, ...) \
	SPDLOG_LOGGER_CALL(FLogManager::GetLogger(Category), FLogManager::ConvertVerbosity(ELogVerbosity::Verbosity), TEXT(Format), __VA_ARGS__)
