#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#include "Renderwerk/Application/Application.h"
#include "Renderwerk/Memory/UniquePointer.h"

#if RW_CONFIG_DEBUG
#	define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
# elif RW_CONFIG_DEVELOPMENT
#	define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#else
#	define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_WARN
#endif

#include "spdlog/async_logger.h"
#include "spdlog/spdlog.h"

#include <memory>

class ENGINE_API FLogManager
{
public:
	static void Initialize();
	static void Shutdown();

public:
	[[nodiscard]] static std::shared_ptr<spdlog::async_logger> GetMainLogger();

private:
	static std::shared_ptr<spdlog::async_logger> MainLogger;
};

#define RW_LOG_TRACE(Message, ...) SPDLOG_LOGGER_TRACE(FLogManager::GetMainLogger(), Message, __VA_ARGS__)
#define RW_LOG_DEBUG(Message, ...) SPDLOG_LOGGER_DEBUG(FLogManager::GetMainLogger(), Message, __VA_ARGS__)
#define RW_LOG_INFO(Message, ...) SPDLOG_LOGGER_INFO(FLogManager::GetMainLogger(), Message, __VA_ARGS__)
#define RW_LOG_WARN(Message, ...) SPDLOG_LOGGER_WARN(FLogManager::GetMainLogger(), Message, __VA_ARGS__)
#define RW_LOG_ERROR(Message, ...) SPDLOG_LOGGER_ERROR(FLogManager::GetMainLogger(), Message, __VA_ARGS__)
#define RW_LOG_CRITICAL(Message, ...) SPDLOG_LOGGER_CRITICAL(FLogManager::GetMainLogger(), Message, __VA_ARGS__)
