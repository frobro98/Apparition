#pragma once

// Logging System Notes
// 
// Main log system gets all logging, uses fmtlib, can do color in console,
// writes to file, set log level, different "sinks"

// Design of log tool
// Needs to initialize specific "sinks" e.g. allocate console, debug window output,
// open file for writing, in-game console, etc.

#include "BasicTypes/Intrinsics.hpp"
WALL_WRN_PUSH
#include <spdlog/spdlog.h>
WALL_WRN_POP
#include "CoreAPI.hpp"
#include "Logging/LogChannel.hpp"

WALL_WRN_PUSH
#include <iterator>
#include <memory>
WALL_WRN_POP

DEFINE_LOG_CHANNEL(DefaultLog);

// TODO - Not really cleaning things up currently. If I delete the thread, then the os thread execution
// will be using dealloced memory. Need a better way of cleaning things up potentially
class CORE_API Logger
{
public:
	void InitLogging(spdlog::level::level_enum level);

	template <typename... Args>
	forceinline void Log(const LogChannel& logChannel, spdlog::level::level_enum level, const tchar* msg, Args&&... args)
	{
		if (isInitialized)
		{
			LogInternal(logChannel, level, msg, fmt::make_format_args(args...));
		}
	}

	forceinline void SetLogLevel(spdlog::level::level_enum level)
	{
		spdlog::set_level(level);
	}
	forceinline spdlog::level::level_enum GetLogLevel() const
	{
		return spdlog::get_level();
	}

private:
	void LogInternal(const LogChannel& logChannel, spdlog::level::level_enum level, const tchar* msg, const fmt::format_args& formatArgs);

private:
	std::shared_ptr<spdlog::logger> logger;
	bool isInitialized = false;
};

CORE_API Logger& GetLogger();






