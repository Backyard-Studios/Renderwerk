#include "pch.h"

#include "Renderwerk/Core/LogManager.h"

#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::async_logger> FLogManager::MainLogger;

void FLogManager::Initialize()
{
	spdlog::init_thread_pool(8192, 1);
	spdlog::sinks_init_list Sinks = {
		std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
	};

	MainLogger = std::make_shared<spdlog::async_logger>("Renderwerk", Sinks, spdlog::thread_pool());

	register_logger(MainLogger);
	set_default_logger(MainLogger);
	spdlog::set_pattern("%^[%T.%e] [%t] [%n] %8l:%$ %v");

#if RW_CONFIG_DEBUG
	spdlog::set_level(spdlog::level::trace);
#elif RW_CONFIG_DEVELOPMENT
	spdlog::set_level(spdlog::level::debug);
#else
	spdlog::set_level(spdlog::level::warn);
#endif
}

void FLogManager::Shutdown()
{
	spdlog::shutdown();
}

std::shared_ptr<spdlog::async_logger> FLogManager::GetMainLogger()
{
	return MainLogger;
}
