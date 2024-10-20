#include "pch.h"

#include "Renderwerk/Logging/LogManager.h"

#include "spdlog/async.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

DEFINE_LOG_CATEGORY(LogDefault);

#define LOG_PATTERN "%^[%T.%e] [%t] [%n] %8l:%$ %v"

FMutex FLogManager::Mutex;

void FLogManager::RegisterLogCategory(const ILogCategory& Category)
{
	if (spdlog::get(Category.GetName().c_str()) != nullptr)
		return;
	FScopedLock Lock(Mutex);
	spdlog::sinks_init_list Sinks = {
		std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
	};

	TSharedPtr<FAsyncLogger> Logger = MakeShared<FAsyncLogger>(Category.GetName().c_str(), Sinks, spdlog::thread_pool());
	Logger->set_level(ConvertVerbosity(Category.GetVerbosity()));
	Logger->set_pattern(LOG_PATTERN);
	register_logger(Logger);
}

TSharedPtr<spdlog::logger> FLogManager::GetLogger(const ILogCategory& Category)
{
	if (spdlog::get(Category.GetName().c_str()) == nullptr)
		RegisterLogCategory(Category);
	FScopedLock Lock(Mutex);
	return spdlog::get(Category.GetName().c_str());
}

void FLogManager::Initialize()
{
	RW_PROFILING_MARK_FUNCTION();

	spdlog::init_thread_pool(8192, 1);
	spdlog::set_level(ConvertVerbosity(SPDLOG_ACTIVE_LEVEL));
	spdlog::set_pattern(LOG_PATTERN);

	RegisterLogCategory(LogDefault);
}

void FLogManager::Shutdown()
{
	RW_PROFILING_MARK_FUNCTION();

	spdlog::shutdown();
}

spdlog::level::level_enum FLogManager::ConvertVerbosity(const ELogVerbosity& Verbosity)
{
	switch (Verbosity)
	{
	case ELogVerbosity::Fatal: return spdlog::level::critical;
	case ELogVerbosity::Error: return spdlog::level::err;
	case ELogVerbosity::Warn: return spdlog::level::warn;
	case ELogVerbosity::Info: return spdlog::level::info;
	case ELogVerbosity::Debug: return spdlog::level::debug;
	case ELogVerbosity::Trace:
	default:
		return spdlog::level::trace;
	}
}

spdlog::level::level_enum FLogManager::ConvertVerbosity(const uint32 Verbosity)
{
	switch (Verbosity)
	{
	case SPDLOG_LEVEL_CRITICAL: return spdlog::level::critical;
	case SPDLOG_LEVEL_ERROR: return spdlog::level::err;
	case SPDLOG_LEVEL_WARN: return spdlog::level::warn;
	case SPDLOG_LEVEL_INFO: return spdlog::level::info;
	case SPDLOG_LEVEL_DEBUG: return spdlog::level::debug;
	case SPDLOG_LEVEL_TRACE: return spdlog::level::trace;
	default:
		return spdlog::level::warn;
	}
}
