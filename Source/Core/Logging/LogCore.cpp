
#include "LogCore.hpp"
#include "Debugging/Assertion.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

Logger& GetLogger()
{
	static Logger logger;
	return logger;
}

void Logger::InitLogging(spdlog::level::level_enum level)
{
	// TODO - Preserve existing log files via file sink handlers
	auto msvcDebugSink = std::make_shared<spdlog::sinks::msvc_sink_st>();
	constexpr bool truncate = true;
	auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_st>("logs/log.txt", truncate);
	auto logger = std::make_shared<spdlog::logger>("Logger", spdlog::sinks_init_list{msvcDebugSink, fileSink});

	logger->set_level(level);
	logger->set_pattern("[%H:%M:%S][%l] %v");
	spdlog::set_default_logger(logger);

	isInitialized = true;
}

void Logger::LogInternal(const LogChannel& logChannel, spdlog::level::level_enum level, const fmt::memory_buffer& formatBuffer)
{
    spdlog::log(level, "[{}] {}", logChannel.logName, formatBuffer.data(), formatBuffer.size());
}