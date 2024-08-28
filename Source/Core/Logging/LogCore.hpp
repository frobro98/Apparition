#pragma once

// Logging System Notes
// 
// Main log system gets all logging, uses fmtlib, can do color in console,
// writes to file, set log level, different "sinks"

// Design of log tool
// Needs to initialize specific "sinks" e.g. allocate console, debug window output,
// open file for writing, in-game console, etc.

#include <iterator>

#include "BasicTypes/Intrinsics.hpp"
WALL_WRN_PUSH
#include "fmt/format.h"
WALL_WRN_POP
#include "CoreAPI.hpp"
#include "String/CStringUtilities.hpp"
#include "Logging/LogChannel.hpp"
#include "Logging/LogLevel.hpp"
#include "Logging/LogSink.hpp"

class LoggingThread;

DEFINE_LOG_CHANNEL(DefaultLog);

// TODO - Not really cleaning things up currently. If I delete the thread, then the os thread execution
// will be using dealloced memory. Need a better way of cleaning things up potentially
class CORE_API Logger
{
public:
	void InitLogging(LogLevel::Type level);

	template <typename... Args>
	forceinline void Log(const LogChannel& logChannel, LogLevel::Type level, const tchar* msg, Args&&... args)
	{
		if (ShouldLogAtLevel(level))
		{
			fmt::vformat_to(std::back_inserter(formatBuf), msg, fmt::make_format_args(args...));
			PushLineToLog(logChannel, level, formatBuf.data(), formatBuf.size());
			formatBuf.clear();
		}
	}

	forceinline void Log(LogLevel::Type level, const tchar* msg)
	{
		Log(DefaultLog, level, msg);
	}

	forceinline void Log(const LogChannel& logChannel, LogLevel::Type level, const tchar* msg)
	{
		if (ShouldLogAtLevel(level))
		{
			PushLineToLog(logChannel, level, msg, Strlen(msg));
		}
	}

	forceinline bool ShouldLogAtLevel(LogLevel::Type level)
	{
		return level >= logLevel;
	}

	forceinline void SetLogLevel(LogLevel::Type level)
	{
		logLevel = level;
	}
	forceinline LogLevel::Type GetLogLevel() const
	{
		return logLevel;
	}

	void AddLogSink(LogSink* sink);
	void RemoveLogSink(LogSink* sink);

private:
	void PushLineToLog(const LogChannel& logChannel, LogLevel::Type level, const tchar* msg, size_t msgSize);

private:
	// Threading!
	LoggingThread* loggingThread{ nullptr };
	fmt::memory_buffer formatBuf{};
	LogLevel::Type logLevel{ LogLevel::Info };
};

CORE_API Logger& GetLogger();






